read_hard_disk:
    ; read a sector from hard disk
    ; params
    ; ax => LBA(logical block addressing) 0-15bits
    ; cx => LBA 16-28bits
    ; ds:bx => write address for the data read from hard disk

    ; return value
    ; bx=bx+512(a sector is 512 bytes)

    mov dx, 0x1f3
    out dx, al
    
    ;mov dx, 0x1f4
    inc dx
    mov al, ah
    out dx, al

    mov ax, cx
    
    inc dx
    out dx, al

    ; 0x1f6 only need 4 bits address
    inc dx
    mov al, ah
    and al, 0x0f
    or al, 0xe0
    out dx, al

    ; the number of sector you want to read
    mov dx, 0x1f2
    mov al, 1
    out dx, al

    ; aquire to read
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

    .wait_disk:
        in al, dx
        and al, 0x80
        cmp al, 0x80
        jne .wait_disk

    ; read 512 bytes to ds:bx, each loop read a word
    mov cx, 256
    mov dx, 0x1f0
    .read_word:
        in ax, dx
        mov [ds:bx], ax
        add bx, 2
        loop .read_word
    
    ret
