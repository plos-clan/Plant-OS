[BITS 32]
; GLOBAL asm_hlt, asm_cli, asm_sti, asm_stihlt
; GLOBAL asm_in8, asm_in16, asm_in32
; GLOBAL asm_out8, asm_out16, asm_out32
GLOBAL io_load_eflags, io_store_eflags
GLOBAL load_gdtr, load_idtr, loader_main
EXTERN DOSLDR_MAIN                                              ; 在 loader.c 那里定义
EXTERN flint                                                    ; 在 fdc.c 中定义
GLOBAL load_cr0, store_cr0, memtest_sub, null_inthandler
GLOBAL move_cursor_by_idx
GLOBAL floppy_int
[SECTION .text]
%define ADR_BOTPAK 0x100000

;; definition of
;;      int io_load_eflags(void);
io_load_eflags:
	PUSHFD                  ; PUSH EFLAGS
	POP    EAX
	RET

;; definition of
;;      void io_store_eflags(int eflags);
io_store_eflags:
	MOV   EAX, [ESP + 4]
	PUSH  EAX
	POPFD                   ; POP EFLAGS
	RET

;; definition of
;;       void load_gdtr(int limit, int addr);
load_gdtr:
	MOV  AX,        [ESP + 4] ; limit
	MOV  [ESP + 6], AX
	LGDT [ESP + 6]
	RET

;; definition of
;;      void load_idtr(int limit, int addr);
load_idtr:
	MOV  AX,        [ESP + 4] ; limit
	MOV  [ESP + 6], AX
	LIDT [ESP + 6]
	RET

;; definition of
;;      int load_cr0(void)
load_cr0:
	MOV EAX, CR0
	RET

;; definition of
;;      void store_cr0(int cr0);
store_cr0:
	MOV EAX, [ESP + 4]
	MOV CR0, EAX
	RET

;; definition of
;;      void floppy_int(void);
floppy_int:
	cli
	pushad
	mov  eax, esp
	push eax
	call flint
	pop  eax
	popad
	sti
	IRETD

;; definition of
;;      u32 memtest_sub(u32 start, u32 end)
memtest_sub:
	CLI
	PUSH EDI                                 ; （由于还要使用EBX, ESI, EDI）
	PUSH ESI
	PUSH EBX
	MOV  ESI,             0xaa55aa55         ; pat0 = 0xaa55aa55;
	MOV  EDI,             0x55aa55aa         ; pat1 = 0x55aa55aa;
	MOV  EAX,             [ESP + 12 + 4]     ; i = start;
	MOV  dword[testsize], 1024 * 1024 * 1024 ; testsize = 1024 * 1024 * 1024;
mts_loop:
	MOV EBX,         EAX
	ADD EBX,         [testsize]     ; p = i + testsize;
	SUB EBX,         4              ; p - = 4;
	MOV EDX,         [EBX]          ; old = * p;
	MOV [EBX],       ESI            ; * p = pat0;
	XOR DWORD [EBX], 0xffffffff     ; * p ^= 0xffffffff;
	CMP EDI,         [EBX]          ; if ( * p != pat1) goto fin;
	JNE mts_fin
	XOR DWORD [EBX], 0xffffffff     ; * p ^= 0xffffffff;
	CMP ESI,         [EBX]          ; if ( * p != pat0) goto fin;
	JNE mts_fin
	MOV [EBX],       EDX            ; * p = old;
	ADD EAX,         [testsize]     ; i + = testsize;
	CMP EAX,         [ESP + 12 + 8] ; if (i <= end) goto mts_loop;

	JBE mts_loop
	STI
	POP EBX
	POP ESI
	POP EDI
	RET
mts_fin:
	CMP dword[testsize], 0x1000 ; if (testsize == 0x1000) goto mts_nomore;
	JE  mts_nomore
	SHR dword[testsize], 2      ; testsize / = 4;
	JMP mts_loop
mts_nomore:
	STI
	MOV [EBX], EDX ; * p = old;
	POP EBX
	POP ESI
	POP EDI
	RET

move_cursor_by_idx: ;移动光标
	ret
null_inthandler:
	iretd
memcpy:
	mov eax,   [esi]
	mov [edi], eax
	add esi,   4
	add edi,   4
	dec ecx
	jnz memcpy
	ret

	global _IN
_IN:
	mov  ebx, [esp + 4]
	mov  edx, [esp + 8]
	push ebx
	push edx
	jmp  far [esp]
loader_main: ; 程序的起点
	mov esp, stack_top
	jmp DOSLDR_MAIN
	[SECTION .data]
testsize: dd 0
	[SECTION .bss]
stack: resb 40 * 1024
stack_top:
