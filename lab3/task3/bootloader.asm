%include "boot.inc"

; bootloader的起始地址
; org 0x7e00
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



; 64位描述符每位对应的信息
; 对于低32字节，高16位是段基地址的低16位，而低16位是段界限的低16位
; 对于高32字节，首尾的8位即16位构成段基地址的高16位，而还有4位段界限位于16-19位，其余是粒度、TYPE等信息
; 这里描述符表的地址从0x8800开始，即GDT_START_ADDRESS=0x8800
; 空描述符
mov dword [GDT_START_ADDRESS+0x00], 0x00
mov dword [GDT_START_ADDRESS+0x04], 0x00

; 创建一个数据段描述符，对应0-4G的线性地址空间
mov dword [GDT_START_ADDRESS+0x08], 0x0000ffff
mov dword [GDT_START_ADDRESS+0x0c], 0x00cf9200

; 建立保护模式下的堆栈描述符
mov dword [GDT_START_ADDRESS+0x10], 0x00000000
mov dword [GDT_START_ADDRESS+0x14], 0x00409600

; 建立保护模式下的显存描述符
mov dword [GDT_START_ADDRESS+0x18], 0x80007fff
mov dword [GDT_START_ADDRESS+0x1c], 0x0040920b

; 建立保护模式下的平坦模式代码段描述符，及基地址从0开始
mov dword [GDT_START_ADDRESS+0x20], 0x0000ffff
mov dword [GDT_START_ADDRESS+0x24], 0x00cf9800
here:

; 初始化描述符表寄存器GDTR，里面存放着描述符的基地址和表的界限，一共48位(32+16)
mov word [gdtr], 39
; lgdt指令用于将数据存入gdtr寄存器
lgdt [gdtr]

; 打开第21根地址线
in al, 0x92
or al, 0000_0010B
out 0x92,al

cli ; 关闭中断机制
mov eax, cr0
or eax, 1
mov cr0, eax

jmp dword CODE_SELECTOR:protect_mode_begin

; 16位的描述符选择子，32位偏移
; 清流水线并串行化处理器
; 保护模式下段寄存器中存放这段选择子，描述符索引位高13位，
; 操作系统会到内存的描述符表中根据索引取出线性段基地址，加上偏移地址后得到32位线性地址
[bits 32]
protect_mode_begin:
	mov eax, DATA_SELECTOR
	mov ds, eax
	mov es, eax
	mov eax, STACK_SELECTOR
	mov ss, eax
	mov eax, VIDEO_SELECTOR
	mov gs, eax

	mov ecx, protect_mode_tag_end-protect_mode_tag
	mov ebx, 80*2	; 在第2行开始打印，因为每个位置的属性由两个字节决定所以是*2
	mov esi, protect_mode_tag
	mov ah, 0x3	; 青色
	print2:
		mov al, [esi]	; 变址寻址的方式，对应段为ds
		mov word[gs:ebx], ax
		add ebx, 2
		inc esi
		loop print2

jmp $

gdtr dw 0
dd GDT_START_ADDRESS


bootloader_tag db "linzhiyang host run bootloader"
bootloader_tag_end:

protect_mode_tag db 'linzhy enter protect mode'
protect_mode_tag_end:
