	[bits 32]
	global asm_inthandler, asm_inthandler_quit, asm_sysenter_handler, asm_syscall_quit
	extern inthandler
	
	%define RING3_CS 3 * 8
	%define RING3_DS 4 * 8
	
	section .text
	
asm_inthandler_main:
	push ds
	push es
	push fs
	push gs
	pusha
	mov ax, ss                   ; 这三行里必须要有 因为从 v86 到这里，必须要切换段寄存器
	mov ds, ax                   ; 不要像我一样看不开 kvm 没必要切换就删掉 QwQ
	mov es, ax
	mov ecx, dword [esp + 12 * 4]; arg1 <== id
	mov edx, esp                 ; arg2 <== regs
	call inthandler              ; void inthandler(i32 id, regs32 * regs) __attribute__((fastcall));
asm_inthandler_quit:
	popa
	pop gs
	pop fs
	pop es
	pop ds
	add esp, 8                   ; pop error code and interrupt number
	sti
	iret
asm_into_inthandler:          ; view `handlergen.py` for more information
	sub esp, 4
	push eax
	mov eax, [esp + 8]
	mov [esp + 4], eax
	pop eax
	jmp short asm_inthandler_main
	jmp short asm_into_inthandler
asm_inthandler:               ; DON'T EDIT THE CODE BELOW!
	dd 0xf7e9006a, 0x6affffff, 0xfff0e901, 0x026affff, 0xffffe9e9, 0xe9036aff, 0xffffffe2, 0xdbe9046a
	dd 0x6affffff, 0xffd4e905, 0x066affff, 0xffffcde9, 0xe9076aff, 0xffffffc6, 0xbde9086a, 0x6affffff
	dd 0xffb8e909, 0x0a6affff, 0xffffafe9, 0xe90b6aff, 0xffffffa8, 0xa1e90c6a, 0x6affffff, 0xff9ae90d
	dd 0x0e6affff, 0xffff93e9, 0xe90f6aff, 0xffffff8e, 0x87e9106a, 0x6affffff, 0xff7ee911, 0x126affff
	dd 0xffff79e9, 0xe9136aff, 0xffffff72, 0x6be9146a, 0x6affffff, 0xff62e915, 0x166affff, 0xffff5de9
	dd 0xe9176aff, 0xffffff56, 0x4fe9186a, 0x6affffff, 0xff48e919, 0x1a6affff, 0xffff41e9, 0xe91b6aff
	dd 0xffffff3a, 0x33e91c6a, 0x6affffff, 0xff2ae91d, 0x1e6affff, 0xffff23e9, 0xe91f6aff, 0xffffff1e
	dd 0x17e9206a, 0x6affffff, 0xff10e921, 0x226affff, 0xffff09e9, 0xe9236aff, 0xffffff02, 0xfbe9246a
	dd 0x6afffffe, 0xfef4e925, 0x266affff, 0xfffeede9, 0xe9276aff, 0xfffffee6, 0xdfe9286a, 0x6afffffe
	dd 0xfed8e929, 0x2a6affff, 0xfffed1e9, 0xe92b6aff, 0xfffffeca, 0xc3e92c6a, 0x6afffffe, 0xfebce92d
	dd 0x2e6affff, 0xfffeb5e9, 0xe92f6aff, 0xfffffeae, 0xa7e9306a, 0x6afffffe, 0xfea0e931, 0x326affff
	dd 0xfffe99e9, 0xe9336aff, 0xfffffe92, 0x8be9346a, 0x6afffffe, 0xfe84e935, 0x366affff, 0xfffe7de9
	dd 0xe9376aff, 0xfffffe76, 0x6fe9386a, 0x6afffffe, 0xfe68e939, 0x3a6affff, 0xfffe61e9, 0xe93b6aff
	dd 0xfffffe5a, 0x53e93c6a, 0x6afffffe, 0xfe4ce93d, 0x3e6affff, 0xfffe45e9, 0xe93f6aff, 0xfffffe3e
	dd 0x37e9406a, 0x6afffffe, 0xfe30e941, 0x426affff, 0xfffe29e9, 0xe9436aff, 0xfffffe22, 0x1be9446a
	dd 0x6afffffe, 0xfe14e945, 0x466affff, 0xfffe0de9, 0xe9476aff, 0xfffffe06, 0xffe9486a, 0x6afffffd
	dd 0xfdf8e949, 0x4a6affff, 0xfffdf1e9, 0xe94b6aff, 0xfffffdea, 0xe3e94c6a, 0x6afffffd, 0xfddce94d
	dd 0x4e6affff, 0xfffdd5e9, 0xe94f6aff, 0xfffffdce, 0xc7e9506a, 0x6afffffd, 0xfdc0e951, 0x526affff
	dd 0xfffdb9e9, 0xe9536aff, 0xfffffdb2, 0xabe9546a, 0x6afffffd, 0xfda4e955, 0x566affff, 0xfffd9de9
	dd 0xe9576aff, 0xfffffd96, 0x8fe9586a, 0x6afffffd, 0xfd88e959, 0x5a6affff, 0xfffd81e9, 0xe95b6aff
	dd 0xfffffd7a, 0x73e95c6a, 0x6afffffd, 0xfd6ce95d, 0x5e6affff, 0xfffd65e9, 0xe95f6aff, 0xfffffd5e
	dd 0x57e9606a, 0x6afffffd, 0xfd50e961, 0x626affff, 0xfffd49e9, 0xe9636aff, 0xfffffd42, 0x3be9646a
	dd 0x6afffffd, 0xfd34e965, 0x666affff, 0xfffd2de9, 0xe9676aff, 0xfffffd26, 0x1fe9686a, 0x6afffffd
	dd 0xfd18e969, 0x6a6affff, 0xfffd11e9, 0xe96b6aff, 0xfffffd0a, 0x03e96c6a, 0x6afffffd, 0xfcfce96d
	dd 0x6e6affff, 0xfffcf5e9, 0xe96f6aff, 0xfffffcee, 0xe7e9706a, 0x6afffffc, 0xfce0e971, 0x726affff
	dd 0xfffcd9e9, 0xe9736aff, 0xfffffcd2, 0xcbe9746a, 0x6afffffc, 0xfcc4e975, 0x766affff, 0xfffcbde9
	dd 0xe9776aff, 0xfffffcb6, 0xafe9786a, 0x6afffffc, 0xfca8e979, 0x7a6affff, 0xfffca1e9, 0xe97b6aff
	dd 0xfffffc9a, 0x93e97c6a, 0x6afffffc, 0xfc8ce97d, 0x7e6affff, 0xfffc85e9, 0xe97f6aff, 0xfffffc7e
	dd 0x77e9806a, 0x6afffffc, 0xfc70e981, 0x826affff, 0xfffc69e9, 0xe9836aff, 0xfffffc62, 0x5be9846a
	dd 0x6afffffc, 0xfc54e985, 0x866affff, 0xfffc4de9, 0xe9876aff, 0xfffffc46, 0x3fe9886a, 0x6afffffc
	dd 0xfc38e989, 0x8a6affff, 0xfffc31e9, 0xe98b6aff, 0xfffffc2a, 0x23e98c6a, 0x6afffffc, 0xfc1ce98d
	dd 0x8e6affff, 0xfffc15e9, 0xe98f6aff, 0xfffffc0e, 0x07e9906a, 0x6afffffc, 0xfc00e991, 0x926affff
	dd 0xfffbf9e9, 0xe9936aff, 0xfffffbf2, 0xebe9946a, 0x6afffffb, 0xfbe4e995, 0x966affff, 0xfffbdde9
	dd 0xe9976aff, 0xfffffbd6, 0xcfe9986a, 0x6afffffb, 0xfbc8e999, 0x9a6affff, 0xfffbc1e9, 0xe99b6aff
	dd 0xfffffbba, 0xb3e99c6a, 0x6afffffb, 0xfbace99d, 0x9e6affff, 0xfffba5e9, 0xe99f6aff, 0xfffffb9e
	dd 0x97e9a06a, 0x6afffffb, 0xfb90e9a1, 0xa26affff, 0xfffb89e9, 0xe9a36aff, 0xfffffb82, 0x7be9a46a
	dd 0x6afffffb, 0xfb74e9a5, 0xa66affff, 0xfffb6de9, 0xe9a76aff, 0xfffffb66, 0x5fe9a86a, 0x6afffffb
	dd 0xfb58e9a9, 0xaa6affff, 0xfffb51e9, 0xe9ab6aff, 0xfffffb4a, 0x43e9ac6a, 0x6afffffb, 0xfb3ce9ad
	dd 0xae6affff, 0xfffb35e9, 0xe9af6aff, 0xfffffb2e, 0x27e9b06a, 0x6afffffb, 0xfb20e9b1, 0xb26affff
	dd 0xfffb19e9, 0xe9b36aff, 0xfffffb12, 0x0be9b46a, 0x6afffffb, 0xfb04e9b5, 0xb66affff, 0xfffafde9
	dd 0xe9b76aff, 0xfffffaf6, 0xefe9b86a, 0x6afffffa, 0xfae8e9b9, 0xba6affff, 0xfffae1e9, 0xe9bb6aff
	dd 0xfffffada, 0xd3e9bc6a, 0x6afffffa, 0xfacce9bd, 0xbe6affff, 0xfffac5e9, 0xe9bf6aff, 0xfffffabe
	dd 0xb7e9c06a, 0x6afffffa, 0xfab0e9c1, 0xc26affff, 0xfffaa9e9, 0xe9c36aff, 0xfffffaa2, 0x9be9c46a
	dd 0x6afffffa, 0xfa94e9c5, 0xc66affff, 0xfffa8de9, 0xe9c76aff, 0xfffffa86, 0x7fe9c86a, 0x6afffffa
	dd 0xfa78e9c9, 0xca6affff, 0xfffa71e9, 0xe9cb6aff, 0xfffffa6a, 0x63e9cc6a, 0x6afffffa, 0xfa5ce9cd
	dd 0xce6affff, 0xfffa55e9, 0xe9cf6aff, 0xfffffa4e, 0x47e9d06a, 0x6afffffa, 0xfa40e9d1, 0xd26affff
	dd 0xfffa39e9, 0xe9d36aff, 0xfffffa32, 0x2be9d46a, 0x6afffffa, 0xfa24e9d5, 0xd66affff, 0xfffa1de9
	dd 0xe9d76aff, 0xfffffa16, 0x0fe9d86a, 0x6afffffa, 0xfa08e9d9, 0xda6affff, 0xfffa01e9, 0xe9db6aff
	dd 0xfffff9fa, 0xf3e9dc6a, 0x6afffff9, 0xf9ece9dd, 0xde6affff, 0xfff9e5e9, 0xe9df6aff, 0xfffff9de
	dd 0xd7e9e06a, 0x6afffff9, 0xf9d0e9e1, 0xe26affff, 0xfff9c9e9, 0xe9e36aff, 0xfffff9c2, 0xbbe9e46a
	dd 0x6afffff9, 0xf9b4e9e5, 0xe66affff, 0xfff9ade9, 0xe9e76aff, 0xfffff9a6, 0x9fe9e86a, 0x6afffff9
	dd 0xf998e9e9, 0xea6affff, 0xfff991e9, 0xe9eb6aff, 0xfffff98a, 0x83e9ec6a, 0x6afffff9, 0xf97ce9ed
	dd 0xee6affff, 0xfff975e9, 0xe9ef6aff, 0xfffff96e, 0x67e9f06a, 0x6afffff9, 0xf960e9f1, 0xf26affff
	dd 0xfff959e9, 0xe9f36aff, 0xfffff952, 0x4be9f46a, 0x6afffff9, 0xf944e9f5, 0xf66affff, 0xfff93de9
	dd 0xe9f76aff, 0xfffff936, 0x2fe9f86a, 0x6afffff9, 0xf928e9f9, 0xfa6affff, 0xfff921e9, 0xe9fb6aff
	dd 0xfffff91a, 0x13e9fc6a, 0x6afffff9, 0xf90ce9fd, 0xfe6affff, 0xfff905e9, 0xe9ff6aff, 0xfffff8fe
asm_inthandler_end:           ; DON'T EDIT THE CODE ABOVE!
	
	extern check_memory_permission3, task_abort
asm_sysenter_handler:
	push RING3_DS                ; push ss
	push ecx                     ; push esp
	pushf                        ; push flags
	push RING3_CS                ; push cs
	push edx                     ; push eip
	push 0                       ; push error code
	push 0x36                    ; push interrupt number
	push ds
	push es
	push fs
	push gs
	mov ecx, esi                 ; in sysenter, arg2 ecx replaced with esi
	mov edx, edi                 ; in sysenter, arg3 edx replaced with edi
	pusha                        ; | and arg4, arg5 replaced with ring3 [esp] [esp + 4]
	mov ebx, dword [esp + 17 * 4]; ebx <== ring3 esp
	push 0                       ; ==================================================
	push 8                       ;
	push ebx                     ;
	call check_memory_permission3; If ring3 esp is not accessible, we should abort the task.
	cmp eax, 0                   ;
	je task_abort                ;
	add esp, 12                  ; ==================================================
	mov esi, dword [ebx]         ; esi <== syscall arg4 ring3 [esp]
	mov edi, dword [ebx + 4]     ; edi <== syscall arg5 ring3 [esp + 4]
	mov dword [esp + 4], esi     ; set syscall arg4 esi
	mov dword [esp], edi         ; set syscall arg5 edi
	mov ecx, 0x36                ; arg1 <== id
	mov edx, esp                 ; arg2 <== regs
	call inthandler              ; void inthandler(i32 id, regs32 * regs) __attribute__((fastcall));
asm_syscall_quit:
	popa
	mov esi, ecx                 ; restore esi
	mov edi, edx                 ; restore edi
	pop gs
	pop fs
	pop es
	pop ds
	add esp, 8                   ; pop error code and interrupt number
	pop edx                      ; pop eip
	add esp, 4                   ; pop cs
	popf                         ; pop flags
	pop ecx                      ; pop esp
	add esp, 4                   ; pop ss
	sti
	sysexit
