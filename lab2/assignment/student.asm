; If you meet compile error, try 'sudo apt install gcc-multilib g++-multilib' first

%include "head.include"
; you code here

your_if:
; put your implementation here
mov eax, 0
mov edx, 0
mov ax,word[a1]
cmp ax, 0xc
jge elseif
; if a1 < 12
mov bx, 0x2
idiv bx
add ax, 0x1
mov [if_flag], ax
jmp your_while

; else if a1 < 24
elseif:
cmp ax, 0x18
jge else
mov bx, 0x18
sub bx, ax
imul bx
mov word [if_flag], ax
jmp your_while

else:
shl ax, 0x4
mov word [if_flag], ax


your_while:
; put your implementation here
mov ax, 0
while_loop:
	mov cx, [a2]
	cmp cx, 0xc
	jl end2
	call my_random
	mov ebx, [a2]
	sub ebx, 0xc
	mov esi, [while_flag]
	mov [ebx+esi], eax

	mov [a2], ebx
	jmp while_loop
end2:


%include "end.include"

your_function:
; put your implementation here
mov esi, [your_string]
for_loop:
	cmp [esi], byte 0
	je end3
	pushad
	mov al, [esi]
	push ax
	call print_a_char
	pop ax
	popad
	inc esi
	jmp for_loop
end3:
