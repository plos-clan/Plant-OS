;;; FAT16BOOT
;;; 该文件是 MBR 扇区的 NASM 写法，编译产物即 MBR 扇区
;;; 前 446 字节为引导程序，后 64 字节为分区表，最后 2 字节为魔数
;;; MBR 扇区共 512 字节，所以本文件的编译产物也应该是 512 字节
;;; 简称：
;;;     fat: 文件分配表
;;;     chs: 柱面—磁头—扇区

; BIOS 会将这个扇区读到 7c00h 地址并开始执行，所以这就是本引导在内存中的位置的段寄存器表示
bootseg  equ 7c0h
dataseg  equ 1000h
readsize equ 200                          ; 单位是扇区
%define e_entry                24
%define e_phoff                28
%define e_phentsize            42
%define e_phnum                44

%define p_offset               4
%define p_vaddr                8
%define p_filesz               16
%define p_memsz                20

%define oem                    "POWERINT" ; OEM 名称（存储介质生产厂商）
%define bytes_per_sector       512        ; 每个扇区的字节数
%define sectors_per_clustor    1          ; 每簇扇区数
%define reserved_sectors_num   1          ; 保留扇区数
%define fat_num                2          ; 文件分配表数目
%define max_rootdir_files_num  224        ; 最大根目录文件个数，32 的倍数
%define max_sectors_num        2880       ; 总扇区数，这里暂且是 1.44 MB ，即一张软盘的容量
%define medium_type            0xf0       ; 介质描述，0xf0 一般表示任意可移动存储介质
%define sectors_per_fat        9          ; 每个文件分配表的扇区
%define sectors_per_track      18         ; 每磁道的扇区数
%define head_num               2          ; 磁头数
%define hidden_sector_num      0          ; 隐藏扇区
%define max_sectors_num_4bytes 2880       ; 总扇区数，如果上面的那个总扇区数为0则使用这里的
                                          ;     否则使用那里的两个字节
%define sign                   0x29       ; 签名
%define id                     0xffffffff
%define volume_label           "POWERINTDOS"    ; 卷标
%define fat_version            "FAT16   "       ; FAT 版本号
%define now_track              0                ; 当前磁头

jmp short start                           ; 跳过 FAT12/FAT16 文件系统定义，开始引导程序

;; 以下为 FAT12/FAT16 文件系统定义
        nop                               ; 依规范，前三个字节是跳转指令
                                          ;     由于我们使用了两字节的短跳，则再补一个空指令
        db oem
        dw bytes_per_sector
        db sectors_per_clustor
        dw reserved_sectors_num
        db fat_num
        dw max_rootdir_files_num
        dw max_sectors_num
        db medium_type
        dw sectors_per_fat
        dw sectors_per_track
        dw head_num
        dd hidden_sector_num
        dd max_sectors_num_4bytes
label_drives_num:
        db 0                              ; 物理驱动器编号，我们会在稍后赋予其值
        db now_track                      ; 当前磁头
        db sign
        dd id
        db volume_label
        db fat_version

;; 主引导程序
start:
        ; 在使用寄存器前清空它们
        xor cx, cx
        xor dx, dx

        mov ax,           bootseg         ; 此后地址[n]表示7c00h（本程序在内存中的位置）+ n
        mov ds,           ax
        mov ax,           dataseg
        mov es,           ax

        mov [0],          dl              ; 计算器启动时会在 dl 寄存器上置当前存储器编号
        mov byte[label_drives_num], dl    ; 现在可以给留空的物理驱动器编号赋值了


        ;; 计算扇区数
        mov ax, max_rootdir_files_num
        ; max_rootdir_files_num / 32 * fat_num == max_rootdir_files_num / (32 / fat_num)
        mov bx, 32 / fat_num
        div bx                            ; 结果在 ax 中
        inc ax                            ; 加上引导扇区
        mov cl, fat_num

.add_sectors_num_for_fat:                 ; 对于每个文件分配表，给 ax 加上其本身占用的扇区数
        add  ax,           sectors_per_fat
        loop .add_sectors_num_for_fat


        cmp byte[label_drives_num], 0x80 ; 判断该存储介质是否是硬盘
        jne .calculate_chs               ; 如果不是，则在 ch、dh、cl 存储柱面，磁头，扇区数
        mov  [packet.lba], ax            ; 为了性能，否则我们使用 LBA（逻辑区块地址）

.lba_read_loop:
        cmp  cl,                readsize
        jae  into_protectmode
        call read1sector
        add  word[packet.seg],  92*512/16; 这么写效率最好
        add  dword[packet.lba], 92
        add  cl,                92
        jmp  .lba_read_loop
.calculate_chs:
        mov bl, 2*18
        div bl
        mov ch, al   ; cyline
        mov al, ah
        xor ah, ah
        mov bl, 18
        div bl
        mov dh, al   ; header
        mov cl, ah   ; sector
        inc cl
.readloop:
        call read1sector
        inc  cl
        cmp  cl, 18+1
        jne  .next
        mov  cl, 1
        inc  dh
        cmp  dh, 1+1
        jne  .next
        xor  dh, dh
        inc  ch
.next:
        mov ax,         es
        add ax,         20h
        mov es,         ax
        inc byte[read]
        cmp byte[read], readsize
        jne .readloop

;; 进入保护模式
into_protectmode:
        ; 1.让CPU支持1M以上内存、设置A20GATE
        in  al,  92h
        or  al,  00000010b
        out 92h, al

        ; 2.禁止PIC外部中断
        cli

        ; 3.设置临时GDT、切换保护模式
        xor  eax,            eax
        mov  ax,             ds
        shl  eax,            4
        mov  dword[GDTR0+2], GDT0
        add  [GDTR0+2],      eax
        mov  ax,             ds
        shl  ax,             4
        add  ax,             in_protectmode
        mov  [GDT0+3*8+2],   ax            ; 初始化inprotectmode代码GDT
        lgdt [GDTR0]
        mov  eax,            cr0
        or   eax,            1
        mov  cr0,            eax
        mov  esp,            0x00ffffff
        ; 4.跳转到保护模式
        jmp  dword 3*8:0

read1sector:
; 读取1个扇区的通用程序
        cmp byte[label_drives_num], 0
        je  .floppy
        cmp byte[label_drives_num], 0x80
        je  .hard
.floppy:
        xor di, di
.retry:
        mov ah, 02h
        mov al, 1
        xor bx, bx
        xor dl, dl
        int 13h
        jnc .readok
        inc di
        mov ah, 00h
        xor dl, dl
        int 13h
        cmp di, 5
        jne .retry
        jmp $
.readok:
        ret
.hard:
        mov ah, 42h
        mov dl, [label_drives_num]
        mov si, packet
        int 13h
        ret

;; 已经进入保护模式之后
[BITS 32]
in_protectmode:
        mov ax, DataGDT
        mov ds, ax
        mov es, ax
        ;mov        fs,ax
        ;mov gs,ax
        mov ss, ax


        ; 接下来，我们要复制Program Header到正确的内存地址

        xor eax,     eax
        mov ax,      dataseg                     ; 取出文件段地址
        shl eax, 4                               ; 段地址*16+偏移地址 = 物理地址
                                                 ; eax 此时是Loader的起始地址
        xor ecx,     ecx
        mov word cx, [eax+e_phnum]               ; 获取Program Header的数量
        xor edx,     edx
        mov word dx, [eax+e_phentsize]           ; 获取一个Program header的大小
                                                 ; 接下来，将ebx设置为第一个程序头的起始地址
        mov ebx,     [eax+e_phoff]
        ; 因为只是偏移量， 所以要加上eax才能表示其在内存中的位置
        add ebx,     eax
.loop_:
        cmp  dword [ebx+p_filesz], 0             ;如果file_sz是0，那么就没必要复制
        je   .skip1                              ; 直接结束
        mov  edi,                  [ebx+p_vaddr] ; edi是dest  vaddr是这个Program Header应被装载到的地址
        push eax
        add  eax,                  [ebx+p_offset]; *(ebx+p_offset) 是这个Program Header在这个程序头的偏移
                                                 ; eax则是文件在内存中的起始地址，所以此时eax是Program Header在内存中的位置
        mov esi, eax                             ; 下面memcpy的src
                push ecx
                mov  ecx, [ebx+p_filesz]         ; 要复制几个字节？[ebx+p_filesz]指的是这个Program Header的大小
                        push ecx
                        call memcpy              ; 上面已经设置了src和dest，所以这里直接复制
                        pop  ecx
                mov  esi, [ebx+p_memsz]          ; 我们现在要处理bss段，于是先获取这个Program Header在内存中的大小
                cmp  ecx, esi                    ; 如果这个Program Header的大小比它在内存中的大小小，说明是bss段，要处理
                jnb  .1                          ; 这个Program Header的大小比它在内存中的大小大，不处理
                sub  esi, ecx                    ; 获取剩下没复制的大小
                mov  ecx, esi
                call memset                      ; memcpy已经将edi赋值到了p_vaddr+p_filesz的位置，直接调用

.1              pop ecx
        pop eax
.skip1:
        add  ebx, edx
        loop .loop_

.skip:
        ; 4.跳转
        finit
        ;cvttss2si eax, [esp+4]
        mov  eax, [eax+e_entry]
        ;sub eax,0x100000
        mov  esp, 0x00ffffff
        push 4*8
        push eax
        jmp  far [esp]

memcpy:
        ; esi: src
        ; edi: dest
        mov al,    [esi]
        mov [edi], al
        inc esi
        inc edi
        dec ecx
        jnz memcpy
        ret
memset:
        ; edi: dest
        mov byte [edi], 0
        inc edi
        dec ecx
        jnz memset
        ret
read: db 0
packet:
        db 10h
        db 0
        dw 92
.off:
        dw 0
.seg:
        dw dataseg
.lba:
        dd 0
        dd 0
; 临时用GDT
GDT0:
NullGDT equ $-GDT0
        dd 0,0
DataGDT equ $-GDT0
        ; 全局数据GDT
        dw 0xffff
        dw 0
        db 0
        db 0x92
        db 0xcf
        db 0
Code1GDT equ $-GDT0
        ; 跳转代码GDT
        dd 0,0
Code2GDT equ $-GDT0
        ; inprotectmode代码GDT
        dw 0xffff
        dw 0
        db 0
        db 0x9a
        db 0xcf
        db 0
Code3GDT equ $-GDT0
        ; 跳转代码GDT
        dw 0xffff
        dw 0
        db 0
        db 0x9a
        db 0xcf
        db 0
GDTR0:
        dw GDTR0-GDT0 ; 临时GDT的大小
        dd GDT0       ; 临时GDT的地址

times   510-($-$$) db 0
db      0x55,0xaa