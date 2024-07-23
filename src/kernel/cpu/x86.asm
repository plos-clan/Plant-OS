[BITS 32]
section .data
		GLOBAL	io_load_eflags, io_store_eflags
		GLOBAL	load_gdtr, load_idtr
		GLOBAL move_cursor_by_idx
		GLOBAL	load_cr0, store_cr0,memtest_sub,farjmp,farcall,start_app
		GLOBAL load_tr
		GLOBAL get_eip,return_to_app,do_init_seg_register,entering_v86
str: db 'Yun Xing Ni Ma De Kernel Xiang Si Shi Bu Shi',0
section .text
%define ADR_BOTPAK 							   0x0
get_eip:		; int get_eip();
	mov	eax,[esp]
	ret
farjmp:		; void farjmp(int eip, int cs);
	pushad
	JMP    FAR [ESP+36]    ; eip, cs
	popad
	RET
farcall:		; void farjmp(int eip, int cs);
	pushad
	call    FAR [ESP+36]    ; eip, cs
	popad
	RET
load_tr:
	ltr [esp+4]
	ret
load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET
EXTERN clear
EXTERN Print_Hex
EXTERN Clear_A_Line

memtest_sub:	; u32 memtest_sub(u32 start, u32 end)
		CLI
		PUSH	EDI						; （由于还要使用EBX, ESI, EDI）
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
		MOV		dword[testsize],1024*1024*1024	; testsize = 1024*1024*1024;
mts_loop:
		pushad
		push eax
		;call Print_Hex
		add esp,4
		;call Clear_A_Line
		popad
		MOV		EBX,EAX
		ADD		EBX,[testsize]		 	; p = i + testsize;
		SUB		EBX,4					; p -= 4;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,[testsize]			; i += testsize;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		
		JBE		mts_loop
		STI
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		CMP		dword[testsize],0x1000	; if (testsize == 0x1000) goto mts_nomore;
		JE		mts_nomore
		SHR		dword[testsize],2	; testsize /= 4;
		JMP		mts_loop
mts_nomore:
		STI
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS 
		POP		EAX
		RET

io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS 
		RET

load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

move_cursor_by_idx:  ;移动光标
mov dx,03d4h;03d4h是索引端口
mov al,0eh;内部的0eh位置存放着光标位置的高八位
out dx,al
inc dx;03d5h是数据端口用于读写数据
in al,dx;读取光标的高八位并且放入bh
mov bh,al
 
dec dx;这儿开始读取光标位置的低八位放入bl
mov al,0fh;0fh位置存放着光标位置的低八位
out  dx,al
inc dx
in al,dx
mov bl,al
 
 
mov word bx,[esp+4]   ;获取参数中的光标位置
 
mov  dx,03d4h;这段代码将改变后的光标位置写入端口内相应的地方以便下次访问
mov al,0eh;写入光标位置高八位
out dx,al
inc dx
mov al,bh
out dx,al
 
dec dx
mov al,0fh    ;写入光标位置低八位
out dx,al
inc dx
mov al,bl
out dx,al
ret

;;蜂鸣器驱动
gensound1:
     push ax
     push bx
     push cx
     push dx
     push di

     mov al, 0b6H
     out 43h, al
     mov dx, 12h
     mov ax, 348ch
     div di
     out 42h, al

     mov al, ah
     out 42h, al

     in al, 61h
     mov ah, al
     or al, 3
     out 61h, al
wait1:
     mov ecx, 3314
     call waitf
delay1:
     dec bx
     jnz wait1

     mov al, ah
     out 61h, al

     pop di
     pop dx
     pop cx
     pop bx
     pop ax
     ret 
;--------------------------
waitf:
      push ax
waitf1:
      in al,61h
      and al,10h
      cmp al,ah
      je waitf1
      mov ah,al
      loop waitf1
      pop ax
      ret

;--------------发声调用函数----------------
GLOBAL gensound
gensound:
      mov edi, [esp+4]
      mov ebx,[esp+8]
	  pusha
      call gensound1
	  popa
      ret

global	init_page
extern	C_init_page
%define	PDE_ADDRESS		0x400000
init_page:	; void init_page(void)
	call	C_init_page
	mov	eax,PDE_ADDRESS
	mov	cr3,eax
	mov	eax,cr0
	or	eax,0x80000000
	mov	cr0,eax	; 开启PAE分页
	ret

GLOBAL get_cpu1
get_cpu1:
	mov eax,0
	DB 0x0F,0xA2
	mov eax,ebx
	ret
GLOBAL get_cpu2
get_cpu2:
	mov eax,0
	DB 0x0F,0xA2
	mov eax,ecx
	ret
GLOBAL get_cpu3
get_cpu3:
	mov eax,0
	DB 0x0F,0xA2
	mov eax,edx
	ret

GLOBAL get_cpu4
get_cpu4:
	mov eax,[esp+4]
	DB 0x0F,0xA2
	mov eax,eax
	ret
GLOBAL get_cpu5
get_cpu5:
	mov eax,[esp+4]
	DB 0x0F,0xA2
	mov eax,ebx
	ret
GLOBAL get_cpu6
get_cpu6:
	mov eax,[esp+4]
	DB 0x0F,0xA2
	mov eax,ecx
	ret
GLOBAL get_cpu7
get_cpu7:
	mov eax,[esp+4]
	DB 0x0F,0xA2
	mov eax,edx
	ret

global __init_PIT
__init_PIT:
    ret
global init_float
init_float:
	cli
	push eax
	FNINIT                                  ; load defaults to FPU
	mov eax,cr0
	and eax,~(1<<2)
	or  eax,(1<<4)
	mov cr0,eax
	pop eax
	sti
	ret
global check
check:
	mov eax,cs      ; 获取cs的值 如果等于0x10则是ldrldr跳转来的
	cmp eax,0x10
	jne .not
	ret
.not:
	mov edx,str
	mov eax,5
	int 36h
	mov eax,0x1e
	int 36h
.hlt:
	jmp .hlt
extern current
global task_switch,task_start
task_switch:
	push ebp
	mov ebp,esp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax
	mov eax,[current]
	mov [eax],esp ; 保存esp
	mov eax,[ebp + 8] ; next
	mov [current],eax 
	mov esp,[eax]
	mov eax,[eax+4]
	mov cr3,eax
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	ret
task_start:
	mov eax,[esp + 4 ] ; next
	mov [current],eax 
	mov esp,[eax]
	mov eax,[eax+4]
	mov cr3,eax
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	ret
return_to_app:
  mov eax,0x56
  int 0x36
  popa
  pop gs
  pop fs
  pop es
  pop ds
  ret
retuen_to_app_end:
do_init_seg_register:
  pusha
  mov ax,1*8
  mov gs,ax
  mov fs,ax
  popa
  ret
; extern void entering_v86(u32 ss, u32 esp, u32 cs, u32 eip);
entering_v86:
   mov ebp, esp               ; save stack pointer

   push dword  [ebp+4]        ; ss
   push dword  [ebp+8]        ; esp
   pushfd                     ; eflags
   or dword [esp], (1 << 17)  ; set VM flags
   push dword [ebp+12]        ; cs
   push dword  [ebp+16]       ; eip
   iret
[SECTION .data]
testsize:	dd	0