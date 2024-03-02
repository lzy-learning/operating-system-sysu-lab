[bits 32]

global asm_hello_world
global asm_lidt
global asm_unhandled_interrupt
global divided_by_zero
global asm_halt

ASM_UNHANDLED_INTERRUPT_INFO db 'Unhandled interrupt happened, halt...'
                             db 

DIVIDED_BY_ZERO_INFO db "Divided by zero interruption triggered by linzhy..."
                    db 0
DIVIDED_BY_ZERO_INFO_END:

ASM_IDTR dw 0
         dd 0

; void asm_unhandled_interrupt()
asm_unhandled_interrupt:
    cli ; 关闭中断机制
    mov esi, ASM_UNHANDLED_INTERRUPT_INFO
    xor ebx, ebx
    mov ah, 0x03
.output_information:
    cmp byte[esi], 0
    je .end
    mov al, byte[esi]
    mov word[gs:bx], ax
    inc esi
    add ebx, 2
    jmp .output_information
.end:
    jmp $

; void divided_by_zero()
divided_by_zero:
    cli
    ;pushad
    mov esi, DIVIDED_BY_ZERO_INFO
    mov ecx, DIVIDED_BY_ZERO_INFO_END-DIVIDED_BY_ZERO_INFO
    xor ebx, ebx
    mov ah, 0x07
    .print:
        mov al, byte[esi]
        mov word[gs:bx], ax
        inc esi
        add ebx, 2
        loop .print
    ;popad
    ;ret
    jmp $


; void asm_lidt(uint32 start, uint16 limit)
asm_lidt:
    push ebp
    mov ebp, esp
    push eax
    ; 根据C语言的函数调用规则，参数从右往左压栈
    mov eax, [ebp + 4 * 3]
    mov [ASM_IDTR], ax
    mov eax, [ebp + 4 * 2]
    mov [ASM_IDTR + 2], eax
    lidt [ASM_IDTR]

    pop eax
    pop ebp
    ret

asm_hello_world:
    push eax
    xor eax, eax

    mov ah, 0x03 ;青色
    mov al, 'H'
    mov [gs:2 * 0], ax

    mov al, 'e'
    mov [gs:2 * 1], ax

    mov al, 'l'
    mov [gs:2 * 2], ax

    mov al, 'l'
    mov [gs:2 * 3], ax

    mov al, 'o'
    mov [gs:2 * 4], ax

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

    pop eax
    ret

asm_halt:
    jmp $