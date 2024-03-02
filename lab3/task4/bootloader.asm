%include "boot.inc"

; org 0x7e00
[bits 16]
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
mov word [gdtr], 39
lgdt [gdtr]
in al, 0x92
or al, 0000_0010B
out 0x92,al
cli
mov eax, cr0
or eax, 1
mov cr0, eax

jmp dword CODE_SELECTOR:protect_mode_begin

[bits 32]
protect_mode_begin:
	mov eax, DATA_SELECTOR
	mov ds, eax
	mov es, eax
	mov eax, STACK_SELECTOR
	mov ss, eax
	mov eax, VIDEO_SELECTOR
	mov gs, eax
	
	; 先把打印信息清除
	mov ecx, 80*25
	mov ah, 0x07
	mov al, ' '
	mov ebx, 0
	clear_screen:
		mov word[gs:ebx], ax
		add ebx, 2
		loop clear_screen
	

	; 字符串长度
	mov ecx,my_id_end-my_id
	; 从第12行第30个位置打印
	mov ebx, 11*80*2+30*2
	mov dh, 0x47
	mov ah, 0x74 
	mov esi, my_id
	print2:
		; 变换颜色
		mov dl, dh
		mov dh, ah
		mov ah, dl
		mov al, [esi]	; 变址寻址的方式，对应段为ds
		mov word[gs:ebx], ax
		add ebx, 2
		inc esi
		loop print2

jmp $

gdtr dw 0
dd GDT_START_ADDRESS


my_id db '21307403lzy'
my_id_end:
