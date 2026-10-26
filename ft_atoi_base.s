section .text
global ft_atoi_base

base_len:
	xor rax, rax
.loop_base_len:
	cmp byte[rdi + rax], 0
	jz .ret
	inc rax
	jmp .loop_base_len
.ret:
	ret


char_index:
	xor rax, rax
.loop_char_index:
	cmp byte[rsi + rax], 0
	jz .ret_min_1
	cmp [rsi + rax], dil
	jz .ret
	inc rax
	jmp .loop_char_index
.ret:
	ret
.ret_min_1:
	mov rax, -1
	ret

base_valid:
	xor rax, rax
	cmp byte[rdi], 0
	jz .ret_zero
	cmp byte[rdi + 1], 0
	jz .ret_zero
.base_valid_loop:
	cmp byte[rdi + rax], 0
	jz .check_duplicate
	cmp byte[rdi + rax], 9
	jz .ret_zero
	cmp byte[rdi + rax], 10
	jz .ret_zero
	cmp byte[rdi + rax], 11
	jz .ret_zero
	cmp byte[rdi + rax], 12
	jz .ret_zero
	cmp byte[rdi + rax], 13
	jz .ret_zero
	cmp byte[rdi + rax], 43
	jz .ret_zero
	cmp byte[rdi + rax], 45
	jz .ret_zero
	cmp byte[rdi + rax], 32
	jz .ret_zero
	inc rax
	jmp .base_valid_loop
.check_duplicate:
	xor rax, rax
.outer:
	cmp byte[rdi + rax], 0
	jz .ret_success
	mov cl, [rdi + rax]
	mov rdx, rax
	inc rdx
.inner:
	cmp byte[rdi + rdx], 0
	jz .next_i
	cmp [rdi + rdx], cl
	jz .ret_zero
	inc rdx
	jmp .inner
.next_i:
	inc rax
	jmp .outer

.ret_zero:
	xor rax, rax
	ret
.ret_success:
	mov rax, 1
	ret


;-----------------------------------------------
;	int ft_atoi_base(char *str, char *base);
;	rbx = str
;	r12 = base
;	r13 = len
;	r14 = number
;	r15 = sign
;-----------------------------------------------
ft_atoi_base:
	push rbx
	push r12
	push r13
	push r14
	push r15
	mov rbx, rdi
	mov r12, rsi
	mov rdi, rsi
	call base_len
	mov r13, rax
	xor r14, r14
	xor r15, r15
	mov rdi, r12
	xor rax, rax
	call base_valid
	test rax, rax
    jz   .zero
    jmp .while_loop
.inc_str_while:
	inc rbx
.while_loop:
	cmp byte[rbx], 9
	je .inc_str_while
	cmp byte[rbx], 10
	je .inc_str_while
	cmp byte[rbx], 11
	je .inc_str_while
	cmp byte[rbx], 12
	je .inc_str_while
	cmp byte[rbx], 13
	je .inc_str_while
	cmp byte[rbx], 32
	je .inc_str_while
	jmp .min_plus
.inc_min:
	inc r15
.inc_plus:
	inc rbx
	jmp .while_atoi
.min_plus:
	cmp byte[rbx], 45
	je .inc_min
	cmp byte[rbx], 43
	je .inc_plus
.while_atoi:
	mov dil, [rbx]
	mov rsi, r12
	call char_index
	cmp eax, -1
	je .done
	imul r14d, r13d
	add r14d, eax
	inc rbx
	jmp .while_atoi
.done:
	mov eax, r14d
	test r15, 1
	jnz .neg_eax
	jmp .exit
.neg_eax:
	neg eax
	jmp .exit
.zero:
    xor  rax, rax
.exit:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    ret

