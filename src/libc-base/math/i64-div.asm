	[bits 32]
	section .text
	global __udivmoddi4, __divmoddi4, __udivdi3, __umoddi3, __divdi3, __moddi3
	
__udivmoddi4:
	push ebp
	push ebx
	push edi
	push esi
	sub esp, 0x18
	mov edi, DWORD [esp + 0x38]
	mov ebx, DWORD [esp + 0x34]
	mov esi, DWORD [esp + 0x30]
	mov ecx, DWORD [esp + 0x2c]
	mov DWORD [esp + 0x4], 0x0
	cmp ebx, ecx
	mov eax, edi
	sbb eax, esi
	jae __udivmoddi4 + 0x82
	xor edx, edx
	mov ebp, 0x1
	nop
	nop
	shld edi, ebx, 0x1
	add ebx, ebx
	shld edx, ebp, 0x1
	add ebp, ebp
	cmp ebx, ecx
	mov eax, edi
	sbb eax, esi
	jb __udivmoddi4 + 0x30
	mov eax, ebp
	or eax, edx
	jne __udivmoddi4 + 0x89
	mov DWORD [esp], 0x0
	mov DWORD [esp + 0x4], 0x0
	mov eax, DWORD [esp + 0x40]
	mov edx, DWORD [esp + 0x3c]
	test edx, edx
	je __udivmoddi4 + 0x71
	mov edi, DWORD [esp]
	mov DWORD [edx], edi
	mov edi, DWORD [esp + 0x4]
	mov DWORD [edx + 0x4], edi
	test eax, eax
	je __udivmoddi4 + 0x7a
	mov DWORD [eax], ecx
	mov DWORD [eax + 0x4], esi
	add esp, 0x18
	pop esi
	pop edi
	pop ebx
	pop ebp
	ret
	mov ebp, 0x1
	xor edx, edx
	mov DWORD [esp], 0x0
	jmp __udivmoddi4 + 0xec
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	sub DWORD [esp + 0xc], esi
	mov esi, ebp
	mov ebp, DWORD [esp + 0x8]
	sbb ebp, eax
	or DWORD [esp + 0x4], ecx
	shrd ebx, edi, 0x1
	mov DWORD [esp + 0x8], ebx
	or DWORD [esp], esi
	mov eax, edx
	mov ebx, edx
	mov edx, DWORD [esp + 0x14]
	shld eax, edx, 0x1f
	shr edi, 1
	mov ecx, ebx
	shr ecx, 1
	mov esi, 0x1
	cmp esi, edx
	mov esi, ebp
	sbb DWORD [esp + 0x10], ebx
	mov ebx, DWORD [esp + 0x8]
	mov ebp, eax
	mov edx, ecx
	mov ecx, DWORD [esp + 0xc]
	jae __udivmoddi4 + 0x59
	mov DWORD [esp + 0x10], 0x0
	mov DWORD [esp + 0xc], ecx
	cmp ebx, ecx
	mov eax, edi
	mov DWORD [esp + 0x8], esi
	sbb eax, esi
	mov eax, edi
	mov esi, ebx
	mov DWORD [esp + 0x14], ebp
	mov ecx, edx
	jb __udivmoddi4 + 0xa0
	xor eax, eax
	xor esi, esi
	xor ebp, ebp
	xor ecx, ecx
	jmp __udivmoddi4 + 0xa0
	
__divmoddi4:
	push ebp
	push ebx
	push edi
	push esi
	sub esp, 0x18
	mov ecx, DWORD [esp + 0x30]
	mov ebp, DWORD [esp + 0x38]
	mov eax, ecx
	sar eax, 0x1f
	xor ecx, eax
	mov edx, DWORD [esp + 0x2c]
	xor edx, eax
	sub edx, eax
	sbb ecx, eax
	mov eax, ebp
	sar eax, 0x1f
	xor ebp, eax
	mov edi, DWORD [esp + 0x34]
	xor edi, eax
	sub edi, eax
	sbb ebp, eax
	mov DWORD [esp + 0x8], 0x0
	cmp edi, edx
	mov eax, ebp
	sbb eax, ecx
	jae __divmoddi4 + 0xe0
	xor esi, esi
	mov ebx, 0x1
	mov eax, ecx
	nop
	nop
	shld ebp, edi, 0x1
	add edi, edi
	shld esi, ebx, 0x1
	add ebx, ebx
	cmp edi, edx
	mov ecx, ebp
	sbb ecx, eax
	jb __divmoddi4 + 0x50
	mov ecx, ebx
	or ecx, esi
	jne __divmoddi4 + 0xe9
	mov DWORD [esp + 0x4], 0x0
	mov DWORD [esp + 0x8], 0x0
	mov ebp, eax
	mov eax, DWORD [esp + 0x40]
	mov ecx, DWORD [esp + 0x3c]
	test ecx, ecx
	mov esi, DWORD [esp + 0x30]
	je __divmoddi4 + 0xbf
	mov ebx, DWORD [esp + 0x38]
	xor ebx, esi
	xor ecx, ecx
	mov edi, edx
	mov esi, DWORD [esp + 0x4]
	mov edx, esi
	neg edx
	mov eax, DWORD [esp + 0x8]
	sbb ecx, eax
	test ebx, ebx
	cmovns edx, esi
	mov esi, DWORD [esp + 0x30]
	cmovns ecx, eax
	mov eax, DWORD [esp + 0x40]
	mov ebx, DWORD [esp + 0x3c]
	mov DWORD [ebx + 0x4], ecx
	mov DWORD [ebx], edx
	mov edx, edi
	test eax, eax
	je __divmoddi4 + 0xd8
	xor ecx, ecx
	mov edi, edx
	neg edx
	sbb ecx, ebp
	test esi, esi
	cmovns edx, edi
	cmovns ecx, ebp
	mov DWORD [eax + 0x4], ecx
	mov DWORD [eax], edx
	add esp, 0x18
	pop esi
	pop edi
	pop ebx
	pop ebp
	ret
	mov eax, ecx
	mov ebx, 0x1
	xor esi, esi
	mov DWORD [esp + 0x4], 0x0
	jmp __divmoddi4 + 0x146
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	sub DWORD [esp + 0xc], ecx
	mov ecx, eax
	mov eax, DWORD [esp + 0x14]
	sbb eax, DWORD [esp]
	or DWORD [esp + 0x8], edx
	shrd edi, ebp, 0x1
	or DWORD [esp + 0x4], ecx
	mov ecx, esi
	shld ecx, ebx, 0x1f
	shr ebp, 1
	mov edx, esi
	shr edx, 1
	mov DWORD [esp], esi
	mov esi, ebx
	mov ebx, 0x1
	cmp ebx, esi
	mov esi, DWORD [esp]
	sbb DWORD [esp + 0x10], esi
	mov ebx, ecx
	mov esi, edx
	mov edx, DWORD [esp + 0xc]
	jae __divmoddi4 + 0x7a
	mov DWORD [esp + 0x10], 0x0
	mov DWORD [esp + 0xc], edx
	cmp edi, edx
	mov ecx, ebp
	mov DWORD [esp + 0x14], eax
	sbb ecx, eax
	mov DWORD [esp], ebp
	mov ecx, edi
	mov eax, ebx
	mov edx, esi
	jb __divmoddi4 + 0x100
	mov DWORD [esp], 0x0
	xor ecx, ecx
	xor eax, eax
	xor edx, edx
	jmp __divmoddi4 + 0x100
	
__udivdi3:
	push ebp
	push ebx
	push edi
	push esi
	sub esp, 0x18
	mov ecx, DWORD [esp + 0x38]
	mov esi, DWORD [esp + 0x34]
	mov ebx, DWORD [esp + 0x30]
	mov edx, DWORD [esp + 0x2c]
	cmp esi, edx
	mov eax, ecx
	sbb eax, ebx
	jae __udivdi3 + 0x70
	xor edi, edi
	mov eax, 0x1
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	shld ecx, esi, 0x1
	add esi, esi
	shld edi, eax, 0x1
	add eax, eax
	cmp esi, edx
	mov ebp, eax
	mov eax, ecx
	sbb eax, ebx
	mov eax, ebp
	jb __udivdi3 + 0x30
	mov eax, ebp
	or eax, edi
	mov eax, ebp
	jne __udivdi3 + 0x77
	mov DWORD [esp + 0x4], 0x0
	mov DWORD [esp + 0x8], 0x0
	mov eax, DWORD [esp + 0x4]
	mov edx, DWORD [esp + 0x8]
	add esp, 0x18
	pop esi
	pop edi
	pop ebx
	pop ebp
	ret
	mov eax, 0x1
	xor edi, edi
	mov DWORD [esp + 0x8], 0x0
	mov DWORD [esp + 0x4], 0x0
	jmp __udivdi3 + 0xd4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	sub ebp, edx
	mov edx, DWORD [esp + 0xc]
	sbb DWORD [esp + 0x10], edx
	or DWORD [esp + 0x8], ebx
	shrd esi, ecx, 0x1
	mov edx, DWORD [esp]
	or DWORD [esp + 0x4], edx
	mov edx, edi
	shld edx, eax, 0x1f
	shr ecx, 1
	mov ebx, edi
	shr ebx, 1
	mov DWORD [esp], edi
	mov edi, eax
	mov eax, 0x1
	cmp eax, edi
	mov eax, DWORD [esp]
	sbb DWORD [esp + 0x14], eax
	mov eax, edx
	mov edx, ebp
	mov edi, ebx
	mov ebx, DWORD [esp + 0x10]
	jae __udivdi3 + 0x60
	mov DWORD [esp + 0x14], 0x0
	mov ebp, edx
	cmp esi, edx
	mov edx, ecx
	mov DWORD [esp + 0x10], ebx
	sbb edx, ebx
	mov DWORD [esp + 0xc], ecx
	mov edx, esi
	mov DWORD [esp], eax
	mov ebx, edi
	jb __udivdi3 + 0x90
	mov DWORD [esp + 0xc], 0x0
	xor edx, edx
	mov DWORD [esp], 0x0
	xor ebx, ebx
	jmp __udivdi3 + 0x90
	
__umoddi3:
	push ebp
	push ebx
	push edi
	push esi
	sub esp, 0x10
	mov ecx, DWORD [esp + 0x30]
	mov ebx, DWORD [esp + 0x2c]
	mov edx, DWORD [esp + 0x28]
	mov edi, DWORD [esp + 0x24]
	xor ebp, ebp
	cmp ebx, edi
	mov eax, ecx
	sbb eax, edx
	jae __umoddi3 + 0x54
	xor ebp, ebp
	mov esi, 0x1
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	shld ecx, ebx, 0x1
	add ebx, ebx
	shld ebp, esi, 0x1
	add esi, esi
	cmp ebx, edi
	mov eax, ecx
	sbb eax, edx
	jb __umoddi3 + 0x30
	mov eax, esi
	or eax, ebp
	jne __umoddi3 + 0x59
	mov eax, edi
	add esp, 0x10
	pop esi
	pop edi
	pop ebx
	pop ebp
	ret
	mov esi, 0x1
	mov DWORD [esp], esi
	jmp __umoddi3 + 0x9b
	nop
	nop
	sub esi, edx
	mov DWORD [esp + 0x8], esi
	sbb eax, edi
	shrd ebx, ecx, 0x1
	mov edx, ebp
	mov esi, DWORD [esp]
	shld edx, esi, 0x1f
	shr ecx, 1
	mov edi, ebp
	shr edi, 1
	mov DWORD [esp + 0x4], ebp
	mov ebp, 0x1
	cmp ebp, esi
	mov esi, DWORD [esp + 0x4]
	sbb DWORD [esp + 0xc], esi
	mov DWORD [esp], edx
	mov ebp, edi
	mov edi, DWORD [esp + 0x8]
	mov edx, eax
	jae __umoddi3 + 0x4a
	mov DWORD [esp + 0xc], 0x0
	mov esi, edi
	cmp ebx, edi
	mov edi, ecx
	mov eax, edx
	sbb edi, edx
	mov edi, ecx
	mov edx, ebx
	jb __umoddi3 + 0x60
	xor edi, edi
	xor edx, edx
	jmp __umoddi3 + 0x60
	
__divdi3:
	push ebp
	push ebx
	push edi
	push esi
	sub esp, 0x1c
	mov ecx, DWORD [esp + 0x34]
	mov eax, ecx
	sar eax, 0x1f
	mov edi, ecx
	xor edi, eax
	mov edx, DWORD [esp + 0x30]
	xor edx, eax
	sub edx, eax
	sbb edi, eax
	mov ebx, DWORD [esp + 0x3c]
	mov eax, ebx
	sar eax, 0x1f
	mov DWORD [esp + 0x14], ebx
	xor ebx, eax
	mov ebp, DWORD [esp + 0x38]
	xor ebp, eax
	sub ebp, eax
	sbb ebx, eax
	cmp ebp, edx
	mov eax, ebx
	sbb eax, edi
	jae __divdi3 + 0xa0
	xor esi, esi
	mov ecx, 0x1
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	shld ebx, ebp, 0x1
	add ebp, ebp
	shld esi, ecx, 0x1
	add ecx, ecx
	cmp ebp, edx
	mov eax, ebx
	sbb eax, edi
	jb __divdi3 + 0x50
	mov eax, ecx
	or eax, esi
	jne __divdi3 + 0xa7
	mov DWORD [esp], 0x0
	mov DWORD [esp + 0x4], 0x0
	xor edx, edx
	mov edi, DWORD [esp]
	mov ecx, edi
	neg ecx
	mov esi, DWORD [esp + 0x4]
	sbb edx, esi
	mov eax, DWORD [esp + 0x14]
	xor eax, DWORD [esp + 0x34]
	cmovns edx, esi
	cmovns ecx, edi
	mov eax, ecx
	add esp, 0x1c
	pop esi
	pop edi
	pop ebx
	pop ebp
	ret
	mov ecx, 0x1
	xor esi, esi
	mov DWORD [esp + 0x4], 0x0
	mov DWORD [esp], 0x0
	jmp __divdi3 + 0x106
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	sub DWORD [esp + 0x10], eax
	mov eax, DWORD [esp + 0x8]
	sbb eax, DWORD [esp + 0xc]
	mov DWORD [esp + 0x8], eax
	or DWORD [esp + 0x4], edx
	shrd ebp, ebx, 0x1
	or DWORD [esp], edi
	mov eax, esi
	shld eax, ecx, 0x1f
	shr ebx, 1
	mov edx, esi
	shr edx, 1
	mov edi, ecx
	mov ecx, 0x1
	cmp ecx, edi
	mov edi, DWORD [esp + 0x8]
	sbb DWORD [esp + 0x18], esi
	mov ecx, eax
	mov esi, edx
	mov edx, DWORD [esp + 0x10]
	jae __divdi3 + 0x79
	mov DWORD [esp + 0x18], 0x0
	mov DWORD [esp + 0x10], edx
	cmp ebp, edx
	mov eax, ebx
	mov DWORD [esp + 0x8], edi
	sbb eax, edi
	mov DWORD [esp + 0xc], ebx
	mov eax, ebp
	mov edi, ecx
	mov edx, esi
	jb __divdi3 + 0xc0
	mov DWORD [esp + 0xc], 0x0
	xor eax, eax
	xor edi, edi
	xor edx, edx
	jmp __divdi3 + 0xc0
	
__moddi3:
	push ebp
	push ebx
	push edi
	push esi
	sub esp, 0x14
	mov edx, DWORD [esp + 0x2c]
	mov ebx, DWORD [esp + 0x34]
	mov ecx, edx
	sar ecx, 0x1f
	xor edx, ecx
	mov edi, DWORD [esp + 0x28]
	xor edi, ecx
	sub edi, ecx
	sbb edx, ecx
	mov ecx, ebx
	sar ecx, 0x1f
	xor ebx, ecx
	mov ebp, DWORD [esp + 0x30]
	xor ebp, ecx
	sub ebp, ecx
	sbb ebx, ecx
	xor eax, eax
	cmp ebp, edi
	mov ecx, ebx
	sbb ecx, edx
	jae __moddi3 + 0x87
	xor eax, eax
	mov ecx, 0x1
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	shld ebx, ebp, 0x1
	add ebp, ebp
	shld eax, ecx, 0x1
	add ecx, ecx
	cmp ebp, edi
	mov esi, ebx
	sbb esi, edx
	jb __moddi3 + 0x50
	mov esi, ecx
	or esi, eax
	jne __moddi3 + 0x8c
	xor ecx, ecx
	mov eax, edi
	neg eax
	sbb ecx, edx
	cmp DWORD [esp + 0x2c], 0x0
	cmovns eax, edi
	cmovns ecx, edx
	mov edx, ecx
	add esp, 0x14
	pop esi
	pop edi
	pop ebx
	pop ebp
	ret
	mov ecx, 0x1
	mov DWORD [esp], eax
	jmp __moddi3 + 0xdb
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	sub eax, edi
	mov DWORD [esp + 0x8], eax
	sbb ecx, esi
	mov DWORD [esp + 0x4], ecx
	shrd ebp, ebx, 0x1
	mov esi, DWORD [esp]
	mov ecx, esi
	mov edx, DWORD [esp + 0x10]
	shld ecx, edx, 0x1f
	shr ebx, 1
	mov edi, esi
	shr edi, 1
	mov eax, 0x1
	cmp eax, edx
	sbb DWORD [esp + 0xc], esi
	mov DWORD [esp], edi
	mov edx, DWORD [esp + 0x4]
	mov edi, DWORD [esp + 0x8]
	jae __moddi3 + 0x6a
	mov DWORD [esp + 0x10], ecx
	mov DWORD [esp + 0xc], 0x0
	mov eax, edi
	cmp ebp, edi
	mov esi, ebx
	mov ecx, edx
	sbb esi, edx
	mov esi, ebx
	mov edi, ebp
	jb __moddi3 + 0xa0
	xor esi, esi
	xor edi, edi
	jmp __moddi3 + 0xa0
