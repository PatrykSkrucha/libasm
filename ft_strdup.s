section .text
global ft_strdup
extern ft_strlen
extern ft_strcpy
extern malloc

ft_strdup:
	push rdi
	call ft_strlen
	add rax, 1
	mov rdi, rax
	call malloc wrt ..plt
	cmp rax, 0
	jz .malloc_error
	mov rdi, rax
	mov rsi, [rsp]
	call ft_strcpy
	pop rdi
	ret

.malloc_error:
	pop rdi
	mov rax, 0
	ret

section .note.GNU-stack noalloc noexec nowrite progbits