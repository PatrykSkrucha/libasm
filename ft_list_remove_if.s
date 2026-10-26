section .text
global ft_list_remove_if
extern free

;---------------------------------------------------------------------------
;	void ft_list_remove_if(t_list **begin_list, void *data_ref,
;						   int (*cmp)(), void (*free_fct)(void *));
;
;	rbx = **begin_list
;	r12 = *data_ref
;	r13 = cmp
;	r14 = free_fct
;	r15 = *curr
;	rbp = *prev
;---------------------------------------------------------------------------

ft_list_remove_if:
	push rbx
	push r12
	push r13
	push r14
	push r15
	push rbp
	sub rsp, 8 ;stack alignment
	mov rbx, rdi
	mov r12, rsi
	mov r13, rdx
	mov r14, rcx
	mov r15, [rdi] ;curr = *begin_list
	mov rbp, 0 ;prev = NULL
.while:
	cmp r15, 0
	je .done
.if_call:
	mov rdi, [r15]
	mov rsi, r12
	call r13
	test eax, eax
	jnz .if_else_call
	test rbp, rbp
	jz .inner_else
	mov rax, [r15 + 8]
	mov [rbp + 8], rax
	mov rdi, [r15]
	call r14
	mov rdi, r15
	call free wrt ..plt
	mov r15, [rbp + 8]
	jmp .while
.inner_else:
	mov rax, [r15 + 8]
	mov [rbx], rax
	mov rdi, [r15]
	call r14
	mov rdi, r15
	call free wrt ..plt
	mov r15, [rbx]
	jmp .while
.if_else_call:
	mov rbp, r15
	mov r15, [r15 + 8]
	jmp .while
.done:
	add rsp, 8
	pop rbp
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	ret