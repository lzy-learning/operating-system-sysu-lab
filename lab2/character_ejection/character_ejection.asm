; org 0x7c00
[bits 16]

; clear screen
mov ax, 0xb800
mov ds, ax
mov di, 0
mov ah, 0x07
mov al, ' '
clear:
	cmp di, 0x3e80
	jg end_clear
	mov [ds:di], ax
	add di, 2
	jmp clear 

end_clear:

mov ax, 0
mov ds, ax
pos db 0x1, 0x1

mov bh, 0x0 ; page number
mov dh, 0x2  ; row
mov dl, 0x0  ; col

mov al, '0' ;character
mov bl, 0x0 ;color
mov cx, 0x1 ;number of character

next:
	; set cursor position
	mov ah, 0x2
	int 0x10
	; write character
	mov ah, 0x9
	int 0x10

	add dh, byte[ds:pos]
	add dl, byte[ds:pos+1]

	; update display number
	cmp al, 0x39
	je initialize
	inc al
	jmp end1
	initialize:
		mov al, '0'
	end1:

	; update direction
	cmp dh, 0x0
	jne elseif1
	mov byte[pos], 0x1
	jmp end2
	elseif1:
		cmp dh, 0x18
		jne end2
		mov byte[pos], 0xff
	end2:
	
	cmp dl, 0x0
	jne elseif2
	mov byte[pos+1], 0x1
	jmp end3
	elseif2:
		cmp dl, 0x4f
		jne end3
		mov byte[ds:pos+1], 0xff
	end3:
	inc bl
	mov si, 0x3333
	mov di, 0x3333
	delay1:
		delay2:
			dec si
			cmp si, 0
			nop
			jne delay2
		mov si, 0x1111
		dec di
		cmp di, 0
		jne delay1
	jmp next


times 510 - ($ - $$) db 0x0
db 0x55, 0xaa
