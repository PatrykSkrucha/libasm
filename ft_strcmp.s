section .text
global ft_strcmp

ft_strcmp:
.loop:
	movzx eax, byte [rdi]
	movzx edx, byte [rsi]
	cmp al, dl
	jne .diff
	test al, al
	je .diff
	inc rdi
	inc rsi
	jmp .loop
.diff:
	sub eax, edx
	ret

section .note.GNU-stack noalloc noexec nowrite progbits