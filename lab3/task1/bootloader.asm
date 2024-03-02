; bootloader的起始地址
org 0x7e00
[bits 16]
mov ax, 0xb800
mov gs, ax
mov ah, 0x03
; 利用两个标志求出打印字符串的长度
mov ecx, bootloader_tag_end-bootloader_tag
xor ebx, ebx ;清空ebx
mov esi, bootloader_tag

print:
	mov al, [esi]
	mov word[gs:bx], ax
	inc esi
	add ebx, 2
	loop print

jmp $

bootloader_tag db "linzhiyang host run bootloader"
bootloader_tag_end:
