asio_save_regs:
	mov [edi], ebx
	mov [edi + 4], esi
	mov [edi + 8], edi
	mov [edi + 12], ebp
	mov [edi + 16], esp
	ret
	
asio_restore_regs:
	mov ebx, [edi]
	mov esi, [edi + 4]
	mov edi, [edi + 8]
	mov ebp, [edi + 12]
	mov esp, [edi + 16]
	ret
	
	; void asio_task_do_run(asio_t old, asio_t new);
asio_task_do_run:
	mov [edi], ebx
	mov [edi + 4], esi
	mov [edi + 8], edi
	mov [edi + 12], ebp
	mov [edi + 16], esp
	call [edi + 20]
