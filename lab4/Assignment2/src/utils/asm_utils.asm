[bits 32]

global asm_my_info
info db "21307403+LZY"
info_end:

asm_my_info:
    push esi
    push ebx
    push eax
    mov esi, info           ; 打印字符首地址
    mov ecx, info_end-info  ; 字符串长度
    mov ah, 0x03            ; 字符颜色属性
    xor ebx, ebx            ; 显示屏上的位置初始化为0
    .print_info:
        mov al, byte[esi]
        mov word[gs:bx], ax
        inc esi
        add ebx, 2
        loop .print_info
    pop eax
    pop ebx
    pop esi
    ret

