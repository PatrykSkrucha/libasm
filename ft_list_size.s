section .text
global ft_list_size

ft_list_size:
	xor rax, rax
.loop:
	test rdi, rdi
	jz .return
	inc rax,
	mov rdi, [rdi + 8]
	jmp .loop
.return:
	ret