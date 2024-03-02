%include "boot.inc"

org 0x7c00
[bits 16]
xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov sp, 0x7c00

mov ax, LOADER_START_SECTOR
mov bx, LOADER_START_ADDRESS
mov cx, LOADER_SECTOR_COUNT

; 读取5个扇区大小的bootloader
load_bootloader:
    ; 函数的参数传递规则，传入bootloader的起始扇区和载入内存的位置，按照参数从右往左压入栈中
    push ax
    push bx
    call asm_read_hard_disk
    pop bx
    pop ax
    inc ax
    add bx, 512
    loop load_bootloader

jmp 0x0000:0x7e00
jmp $

asm_read_hard_disk:
    push bp
    mov bp, sp

    push cx
    push dx
    ; 取出booloader起始扇区，因为前面进行了四次压栈，最后一次是push bp
    mov ax, [bp + 2*3]

    mov dx, 0x1f3
    out dx, al
    mov dx, 0x1f4
    mov al, ah
    out dx, al
    xor ax, ax
    inc dx
    out dx, al
    inc dx
    mov al, ah
    and al, 0x0f
    ; 表示从主硬盘(第6位)以LBA方式(第4位)读取数据
    or al, 0xe0
    out dx, al

    ; 决定读取几个扇区
    mov dx, 0x1f2
    mov al, 1
    out dx, al

    ; 向0x1F7端口写入0x20，请求硬盘读
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

    ; 从端口0x1f7判断硬盘是否繁忙
    .waits:
        in al, dx
        and al, 0x88
        cmp al, 0x08
        jnz .waits
    
    ; 读取512字节到指定地址
    mov bx, [bp+2*2]    ; 传入参数在第二位
    mov cx, 256         ; 每次读入一个字两个字节
    mov dx, 0x1f0
    
    .readw:
        in ax, dx
        mov [ds:bx], ax
        add bx, 2
        loop .readw

    pop dx
    pop cx
    pop bp
    ret
    
times 510-($-$$) db 0
db 0x55, 0xaa