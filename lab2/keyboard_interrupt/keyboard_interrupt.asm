; org 0x7c00
[bits 16]
; change display mode to text mode
mov ax, 0x3
int 0x10

mov bh, 0
mov bl, 0x7
mov cx, 1
mov dh, 0
mov dl, 0
loop1:
; wait keyboard enter, and entered character will store in register al
mov ah, 0x0
int 0x16

; check whether usr press key 'esc'
cmp al, 0x1b
je end2

; check key 'backspace'
cmp al, 0x08
jne not_backspace
mov ah, 0x02
dec dl
int 0x10
mov al, ' '
mov ah, 0x09
int 0x10
jmp end


not_backspace:
; check key 'enter'
cmp al, 0xd
jne not_enter
inc dh
mov dl, 0
jmp end



not_enter:
mov ah, 0x02
int 0x10

mov ah, 0x09
int 0x10

; update writing postion
cmp dl, 0x4f
inc dl
jne end
mov dl, 0
inc dh
end:
jmp loop1

end2:
jmp $
times 510 - ($ - $$) db 0
db 0x55, 0xaa

