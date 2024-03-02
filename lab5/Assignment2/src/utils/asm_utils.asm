[bits 32]

global asm_delay
global asm_lidt
global asm_unhandled_interrupt
global asm_halt
global asm_out_port
global asm_in_port
global asm_time_interrupt_handler
global asm_enable_interrupt
global asm_enable_interrupt
global asm_disable_interrupt
global asm_interrupt_status
global asm_switch_thread

extern c_time_interrupt_handler
ASM_UNHANDLED_INTERRUPT_INFO db 'Unhandled interrupt happened, halt...'
                             db 0
ASM_IDTR dw 0
         dd 0

asm_delay:
    push eax
    push ecx
	mov eax, 0xeeee
	mov ecx, 0x6666
    delay1:
		delay2:
			dec eax
			cmp eax, 0
			nop
			jne delay2
		mov eax, 0x1111
		loop delay1
    pop ecx
    pop eax
    ret

; void asm_switch_thread(PCB *cur, PCB *next);
asm_switch_thread:
    push ebp
    push ebx
    push edi
    push esi

    ; 前面压入四个通用寄存器，再加上1等于5，每个4个字节，即可得到PCB* cur
    ; 而int * stack是PCB结构体的第一个变量，所以偏移为0
    mov eax, [esp + 5 * 4 + 0]
    mov [eax], esp ; 保存当前栈指针到PCB中，以便日后恢复，这里[eax]，因为eax本身就保存地址stack
    ; 取出PCB* next，同样的stack在地址上等于next
    mov eax, [esp + 6 * 4]
    mov esp, [eax] ; 此时栈已经从cur栈切换到next栈

    pop esi
    pop edi
    pop ebx
    pop ebp

    sti
    ret

; int asm_interrupt_status();
asm_interrupt_status:
    ; 返回值规定放在eax中
    xor eax, eax
    pushfd  ; 标志寄存器与堆栈栈顶数据的传送
    pop eax
    and eax, 0x200
    ret

; void asm_disable_interrupt();
asm_disable_interrupt:
    cli     ; 禁中断
    ret
; void asm_init_page_reg(int *directory);

asm_enable_interrupt:
    sti     ; 开中断
    ret

asm_time_interrupt_handler:
    pushad
    
    ; 发送EOI消息，否则下一次中断不发生
    mov al, 0x20
    out 0x20, al
    out 0xa0, al
    
    call c_time_interrupt_handler

    popad
    iret

; void asm_in_port(uint16 port, uint8 *value)
asm_in_port:
    push ebp
    mov ebp, esp

    push edx
    push eax
    push ebx

    xor eax, eax
    mov edx, [ebp + 4 * 2] ; port
    mov ebx, [ebp + 4 * 3] ; *value

    in al, dx
    mov [ebx], al

    pop ebx
    pop eax
    pop edx
    pop ebp
    ret

; void asm_out_port(uint16 port, uint8 value)
asm_out_port:
    push ebp
    mov ebp, esp

    push edx
    push eax

    mov edx, [ebp + 4 * 2] ; port
    mov eax, [ebp + 4 * 3] ; value
    out dx, al
    
    pop eax
    pop edx
    pop ebp
    ret

; void asm_unhandled_interrupt()
asm_unhandled_interrupt:
    ; 表示该中断没有处理程序，会打印一段信息并jmp $
    cli
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


; void asm_lidt(uint32 start, uint16 limit)
asm_lidt:
    ; 设置中断向量表的起始地址和界限，即设置中断基地址寄存器
    push ebp
    mov ebp, esp
    push eax

    mov eax, [ebp + 4 * 3]
    mov [ASM_IDTR], ax
    mov eax, [ebp + 4 * 2]
    mov [ASM_IDTR + 2], eax
    lidt [ASM_IDTR]

    pop eax
    pop ebp
    ret

asm_halt:
    jmp $