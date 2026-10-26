section .text
global ft_list_push_front
extern malloc

;-------------------------------------------------------------
;	void ft_list_push_front(t_list **begin_list, void *data)
;-------------------------------------------------------------
ft_list_push_front:
	push rdi
	push rsi
	mov rdi, 16
	sub rsp, 8
	call malloc wrt ..plt
	test rax, rax
	jz .error
	add rsp, 8
	pop rsi
	pop rdi
	mov [rax], rsi
	mov rcx, [rdi]
	mov [rax + 8], rcx 
	mov [rdi], rax
	ret
.error:
	add rsp, 8
	add rsp, 8
	add rsp, 8
	ret