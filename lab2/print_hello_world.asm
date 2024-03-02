; org 0x7c00
[bits 16]

xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov sp, 0x7c00
mov ax ,0xb800
mov gs, ax

mov ah, 0x01
; 11 is length of string "hello world"
mov cx, 11
mov si, 0
mov bx, 0

print_hello_world:
mov al, [si+string]
mov [gs:bx], ax
inc si
add bx, 2
cmp si, cx
jl print_hello_world


jmp $

string db 'hello world'
times 510 - ($-$$) db 0
db 0x55, 0xaa
