; org指令：指定程序起始地址，mbr会将512字节数据复制到0x7c00处执行
; org 0x7c00

; 告诉编译器按16位代码格式编译代码
[bits 16]

xor ax, ax;
; 初始化段寄存器，段地址全部设为0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00
; 这是IA-32处理器将现实矩阵映射到内存地址0xb8000-0xbffff
mov ax, 0xb800
mov gs, ax

; 遵循低位放字符，高位的高4位放背景色，低4位放前景色
; 青色
mov ah, 0x01
mov al, 'H'
; physical address = gs << 4 + 2 * 1
mov [gs:2*0], ax

mov al, 'e'
mov [gs:2*1], ax

mov al, 'l'
mov [gs:2*2], ax

mov al, 'l'
mov [gs:2*3], ax

mov al, 'o'
mov [gs:2*4], ax

mov al, ' '
mov [gs:2 * 5], ax

mov al, 'W'
mov [gs:2 * 6], ax

mov al, 'o'
mov [gs:2 * 7], ax

mov al, 'r'
mov [gs:2 * 8], ax

mov al, 'l'
mov [gs:2 * 9], ax

mov al, 'd'
mov [gs:2 * 10], ax

mov al, ' '
mov [gs:2*11], ax

; $代表当前指令地址，所以这是个死循环
jmp $

; times指令是汇编伪指令，表示重复执行指令若干次
; $表示当前汇编地址，$$表示代码开始的汇编地址
; 下面指令表示填充字符0直到510个字节
times 510 - ($ - $$) db 0

; 最后我们填充0x55,-xaa表示这512字节为mbr
db 0x55, 0xaa


