org 0x7c00
[bits 16]
xor ax, ax

mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov sp, 0x7c00

mov cx, 8
; 设置光标行为10，列从36开始打印，正好打印在屏幕正中央
mov dh, 0x0a
mov dl, 0x23
mov bh, 0
mov bl, 0x07

print:
	push cx
	; 设置光标
	add dl, 1
	mov ah, 0x02
	int 0x10

	; 输出字符
	mov al, [my_id+si]
	mov ah, 0x09
	mov cx, 1
	int 0x10
	inc si
	pop cx
	loop print

jmp $
my_id db "21307403"
times 510 - ($ -$$) db 0
db 0x55, 0xaa
