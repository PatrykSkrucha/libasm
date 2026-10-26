section .text
global ft_list_sort

;-----------------------------------------------------------
;	void ft_list_sort(t_list **begin_list, int (*cmp)());
;
;	rbx = **begin_list
;	r12 = cmp()
;	r13 = *curr
;	r14 = *next
;	r15 = bool sorted
;
;-----------------------------------------------------------

ft_list_sort:
	push rbx
	push r12
	push r13
	push r14
	push r15
	mov rbx, rdi
	mov r12, rsi
	xor r15, r15
.while_outer:
	test r15, r15
	jnz .done
	mov r15, 1
	test rbx, rbx
	jz .done
	mov r13, [rbx]
	test r13, r13
	jz .done
	mov r14, [r13 + 8]
.while_inner:
	test r13, r13
	jz .while_outer
	test r14, r14
	jz .while_outer
	mov rdi, [r13]
	mov rsi, [r14]
	call r12
	test eax, eax
	jg .swap
	jmp .move_curr
.swap:
	mov rax, [r13]
	mov r10, [r14]
	mov [r13], r10
	mov [r14], rax
	mov r15, 0
.move_curr:
	mov r13, r14
	mov r14, [r13 + 8]
	jmp .while_inner
.done:
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	ret
