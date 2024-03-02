; mbr起始地址，即磁盘的第一个扇区
org 0x7c00
[bits 16]
xor ax, ax
; 初始化段寄存器
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00
; 以下类似宏定义的变量定义在boot.inc中
; 分别代表bootloader的起始扇区(1)、加载扇区数量(5)、bootloader起始地址(0x7e00)
mov ax, LOADER_START_SECTOR
mov cx, LOADER_SECTOR_COUNT
mov bx, LOADER_START_ADDRESS
load_bootloader:
	push ax
	push bx
	call read_hard_disk
	add sp, 4
	inc ax
	add bx, 512

	jle load_bootloader

; 前面把bootloader读取到内存中了，接下来跳转到bootloader的起始地址执行
jmp 0x0000:0x7e00

jmp $


read_hard_disk:
	; read a logical sector from hard disk
	; params
	; ax -> 0-15bit of logic sector number
	; cx -> 16-28bit of logic sector number
	; ds:bx -> where read data store
	
	; return value -> bx=bx+15

	mov dx, 0x1f3
	out dx, al
	inc dx
	mov al, ah
	out dx, al
	inc dx
	mov ax, cx
	out dx, al
	inc dx

	mov al, ah
	; 保证高四位是正确的模式，及LBA模式下使用主硬盘
	and al, 0x0f
	or al, 0xe0
	out dx, al

	; 设置读取的扇区数量
	mov dx, 0x1f2
	mov al, 1
	out dx, al
	
	; 设置从主磁盘读命令
	mov dx, 0x1f7
	mov al, 0x20
	out dx, al

	; 等待其他磁盘操作
	; 磁盘状态可以从0x1f7端口读入，第7位是busy位，第3位是drq位，为1则表明硬盘已准备好与主机交换数据
	.wait_disk:
		in al, dx
		and al, 0x88
		cmp al, 0x08
		jne .wait_disk

	; 读取512字节到地址ds:bx，从0x1f0每次读取一个字，两个字节
	mov cx, 256
	mov dx, 0x1f0
	.read_word:
		in ax, dx
		mov [ds:bx], ax
		add bx, 2
		loop .read_word
		
	ret

times 510-($-$$) db 0
db 0x55, 0xaa
