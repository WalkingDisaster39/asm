;======================
;==input/output calls==
;======================


section .data
 str: db 'asdf', 0
 buf: times 8 db 0
 newline: db 0xA
 buffer_read: times 255 db 0
 buffer_write: times 255 db 0

section .text
global _start

check_whitespaces:
	cmp dil, 0xA
	je .whitespace

	cmp dil, 0x9
	je .whitespace

	cmp dil, 0x20
	je .whitespace
	
	cmp dil, 0xA0
	je .whitespace
	
	cmp dil, 0x0D
	je .whitespace
	
.notwhitespace:
	mov rax, 1
	ret
.whitespace:
	mov rax, 0
	ret

_start:
  
mov rdi, str  

string_length:

	xor rcx, rcx
.loop:
	mov dl, [rdi + rcx]
	inc rcx
	test dl, dl
	jne .loop

	dec rcx
	mov rax, rcx
ret

print_string:
   
   call string_length
   
   mov rdx, rax
   mov rsi, rdi
   mov rdi, 1
   mov rax, 1
   syscall
ret 

print_char:
    push rdi
    lea rsi, [rsp]
    mov rdi, 1
    mov rdx, 1
    mov rax, 1
    syscall
    pop rdi
ret

print_newline:

    mov rdi, 0xA 
    syscall print_char
    
ret

print_uint:
   push rbx
   
   mov rax, rdi
   mov rbx, 10
   mov rcx, 0
   
.pushing:
   xor  rdx, rdx
   div  rbx
   add  rdx, 48
   push rdx
   inc  rcx
   test rax, rax
   jnz .pushing
   
.printing:
   pop rdi
   push rcx
   call print_char
   pop rcx
   test rcx, rcx
   dec rcx
   jnz .printing
   
   pop rbx
ret

print_int:
   test rdi, rdi
   jns .abs
   push rdi
   mov  rdi, 45	
   call print_char
   pop rdi	
   neg rdi
.abs:
   call print_uint 
ret   

read_char:
   mov rax, 0
   mov rdi, 0
   mov rsi, buf
   mov rdx, 1
   syscall
ret

read_word:

	mov rax, 0
	mov rdi, 0
	mov rsi, buffer_read
	mov rdx, 255
	syscall
	
	mov r8, 0		;offset of buffer_read
	mov r9, 0		;offset of buffer_write
.check:
	mov  rdi, [buffer_read + r8]
	call check_whitespaces
	test rax, rax
	je   ._space
._word:
	mov [buffer_write + r9], rdi
	inc r8
	inc r9
	
	cmp r8, 255
	je  .exit
	
	jmp .check
._space:
	inc  r8
	cmp  r8, 255
	je .exit
	
	mov  rax, [buffer_write + r9]
	test rax, rax  ;buffer_write filling check
	je  .check
	
.exit:
	mov rdx, r9
	mov rax, buffer_write
ret

parse_uint:
   
        push rbx

	xor  rcx, rcx
	xor  rax, rax
	xor  rdx, rdx
	mov  rbx, 10

.not_digit:
	test rdx, rdx
	jnz  .exit
.digit_check:
	mov  r8b, [buffer + rcx]
	inc rcx
	sub  r8b, 48
	js  .not_digit
	sub  r8b, 10
	jns .not_digit

	add r8b, 10
	inc rdx
	push rdx
	mul rbx
	pop rdx
	add rax, r8
	jmp .digit_check
.exit:
	pop rbx
	ret

parse_int:
	mov rdi, buffer
	call parse_uint
	sub rcx, rdx
	mov r8b, [rdi + rcx - 2]
	cmp r8b, '-'
	je .neg
ret
.neg:
	neg rax
ret

string_equals:

	call string_length
	push rdi
	mov  rdi, rsi
	mov  r8, rax
	call string_length
	
	pop rdi
	
	cmp rax, r8
	jne .not_equal

.check_loop:
	mov dl, [rdi+rax]
	mov r9b, [rsi+rax]
	
	cmp  dl, r9b
	jne .not_equal
	test rax, rax
	jz  .equal

	dec rax
	jmp .check_loop
	
.not_equal:
	xor rax, rax
ret

.equal:	
	mov rax, 1
ret

string_copy:
	call string_length
	xor dl, dl
	mov [rsi+rax], dl
.copy_loop:
	mov dl,[rdi+rax-1]
	mov [rsi+rax-1], dl
	dec rax
	jnz .copy_loop
ret
