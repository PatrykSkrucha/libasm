section .text
global ft_strcpy

ft_strcpy:
	mov rax, rdi
.loop:
	mov dl, [rsi]
	mov [rdi], dl
	inc rdi
	inc rsi
	test dl, dl
	jnz .loop
	ret

section .note.GNU-stack noalloc noexec nowrite progbits