org 0x7c00
[bits 16]
xor ax, ax
; 初始化段寄存器
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

call read_hard_disk

; 前面把bootloader读取到内存中了，接下来跳转到bootloader的起始地址执行
jmp 0x0000:0x7e00

jmp $


read_hard_disk:
	mov ax, 0
	mov es, ax
	mov bx, 0x7e00
	; 中断方式读取磁盘，读取磁盘数量为5，从第0个柱面第0个磁头的第1个扇区开始读取
	; 每次读取检查AH中的状态码是否为00H，如果是说明读取成功，可以返回
	mov ah, 0x02	;功能号
	mov al, 0x05	;读取扇区数，这里bootloader大小是5个扇区
	mov ch, 0	;柱面，从0开始
	mov cl, 2	;扇区
	mov dh, 0	;磁头
	mov dl, 0x80	;驱动器，硬盘从0x80开始
	.read:
		int 0x13;中断，读取硬盘
		cmp ah, 0
		jne .read	; 如果状态码不为0说明读取失败，继续循环
	ret

times 510-($-$$) db 0
db 0x55, 0xaa
