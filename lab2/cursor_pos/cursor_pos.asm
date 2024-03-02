; org 0x7c00
[bits 16]

mov ax, 0xb800
mov gs, ax
mov ax, 0

; 设置光标位置
mov ah, 0x02
mov bh, 0x0
mov dh, 3
mov dl, 3
 int 0x10
mov si, 0


; 获得光标位置
mov ah, 0x03
mov bx, 0
 int 0x10
; 显示结果
mov ah, 0x03
mov bx, 0
mov al, dh
add al, 0x30
mov [gs:bx], ax
add bx, 2
mov al, ' '
mov [gs:bx], ax
mov al, dl
add al, 0x30
add bx, 2
mov [gs:bx], ax


; 在当前光标位置写入xxxx
mov ah, 0x09
mov al, "x"
mov bh, 0
mov bl, 0x01
mov cx, 4
 int 0x10

jmp $

times 510 - ($ - $$) db 0
db 0x55, 0xaa
