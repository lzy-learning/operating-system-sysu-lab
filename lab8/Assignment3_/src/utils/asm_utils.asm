[bits 32]
%include "boot.inc"

global asm_hello_world
global asm_lidt
global asm_unhandled_interrupt  ; 没有中断处理程序的中断
global asm_halt                 ; 阻塞
global asm_out_port             ; 输出端口
global asm_in_port              ; 输入端口
global asm_time_interrupt_handler   ; 捕获时钟中断
global asm_enable_interrupt     ; 开中断
global asm_disable_interrupt    ; 关中断
global asm_interrupt_status     ; 获得中断状态
global asm_switch_thread        ; 切换线程栈
global asm_atomic_exchange      ; 原子操作，交换两个变量
global asm_init_page_reg        ; 设置页目录表首地址，开启分页机制
global asm_system_call          ; 保存5个系统调用参数到寄存器中然后调用系统调用处理函数，通过中断去调用
global asm_system_call_handler  ; 将保存在寄存器中的5个参数压入栈中，然后根据index调用相应处理函数，index作为索引去system_call_table中取
global asm_add_global_descriptor; 在描述符表后添加段描述符，返回段描述符的位置
global asm_ltr                  ; ltr指令，设置tr寄存器的值为TSS的地址
global asm_start_process        ; 执行进程，传入栈地址，然后函数内部修改esp，然后利用pop修改一系列段寄存器
global asm_update_cr3           ; 设置cr3寄存器的值，即页目录表首地址

extern c_time_interrupt_handler
extern system_call_table        ; 注意声明系统调用表来自外面

ASM_UNHANDLED_INTERRUPT_INFO db 'Unhandled interrupt happened, halt...'
                             db 0
ASM_IDTR dw 0
         dd 0

ASM_GDTR dw 0
         dd 0

ASM_TEMP dd 0

; void asm_update_cr3(int address)
asm_update_cr3:
    push eax
    mov eax, dword[esp+8]
    mov cr3, eax
    pop eax
    ret

; void asm_start_process(int stack)
asm_start_process:
    mov eax, dword[esp+4]   ; 取出函数栈
    mov esp, eax            ; 切换函数栈
    popad

    pop gs
    pop fs
    pop es
    pop ds
    iret    ; 中断返回可以从高特权级到低特权级

; void asm_ltr(int tr)
asm_ltr:
    ltr word[esp+1*4]
    ret

; int asm_add_global_descriptor(int low, int high);
asm_add_global_descriptor:
    push ebp
    mov ebp, esp

    push ebx
    push esi

    sgdt [ASM_GDTR]     ; 得到gdtr内容
    mov ebx, [ASM_GDTR+2]   ; GDT地址
    xor esi, esi
    mov si, word[ASM_GDTR]  ; GDT界限
    add esi, 1      ; 往界限后面添加描述符，然后再更新界限
    ; 设置64位的描述符，即[high, low]
    mov eax, [ebp+2*4]
    mov dword[ebx+esi], eax
    mov eax, [ebp+3*4]
    mov dword[ebx+esi+4], eax

    mov eax, esi
    shr eax, 3      ; 返回段描述符在描述符表中的位置

    add word[ASM_GDTR], 8   ; 界限增加8个字节，即32位
    lgdt [ASM_GDTR]

    pop esi
    pop ebx
    pop ebp
    ret


; void asm_system_call_handler
asm_system_call_handler:
    push ds
    push es
    push fs
    push gs
    pushad

    push eax
    ; 栈段会从tss中自动加载
    mov eax, DATA_SELECTOR
    mov ds, eax
    mov es, eax
    mov eax, VIDEO_SELECTOR
    mov gs, eax

    pop eax

    ; 在asm_system_call中将5个参数放进了5个寄存器中
    push edi
    push esi
    push edx
    push ecx
    push ebx
    
    sti
    call dword[system_call_table + eax * 4]
    cli

    ; 将参数出栈
    add esp, 5*4
    mov [ASM_TEMP], eax ; 因为popad会改变eax的值
    popad
    pop gs
    pop fs
    pop es
    pop ds
    mov eax, [ASM_TEMP]

    iret

; int asm_system_call(int index, int first=0, int second=0, int third=0, int forth=0, int fifth=0)
asm_system_call:
    push ebp
    mov ebp, esp

    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ds
    push es
    push fs
    push gs

    mov eax, [ebp + 2*4]
    mov ebx, [ebp + 3*4]
    mov ecx, [ebp + 4*4]
    mov edx, [ebp + 5*4]
    mov esi, [ebp + 6*4]
    mov edi, [ebp + 7*4]

    int 0x80

    pop gs
    pop fs
    pop es
    pop ds
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop ebp

    ret
    
; void asm_init_page_reg(uint32* directory)
asm_init_page_reg:
    push ebp
    mov ebp, esp

    push eax

    mov eax, [ebp+4*2]  ; directory，即页目录表首地址
    mov cr3, eax        ; 放入页目录表首地址
    mov eax, cr0
    or eax, 0x80000000  ; 开启页表机制，设PG=1
    mov cr0, eax

    pop eax
    pop ebp
    ret

; void asm_atomic_exchange(uint32 *register, uint32* memory)
asm_atomic_exchange:
    push ebp
    mov ebp, esp
    pushad

    mov ebx, [ebp + 4*2]    ; &register
    mov eax, [ebx]          ; register
    mov ebx, [ebp+4*3]      ; memory address
    xchg [ebx], eax         ; 原子交换指令，将register的值和[mem]的值交换
    mov ebx, [ebp + 4*2]
    mov [ebx], eax          ; 将eax的值移回register

    popad
    pop ebp
    ret
    
; void asm_switch_thread(PCB *cur, PCB *next);
asm_switch_thread:
    push ebp
    push ebx
    push edi
    push esi

    mov eax, [esp + 5 * 4]
    mov [eax], esp ; 保存当前栈指针到PCB中，以便日后恢复

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
    xor eax, eax
    pushfd
    pop eax
    and eax, 0x200
    ret

; void asm_disable_interrupt();
asm_disable_interrupt:
    cli
    ret
; void asm_init_page_reg(int *directory);

asm_enable_interrupt:
    sti
    ret
asm_time_interrupt_handler:
    pushad
    push ds
    push es
    push fs
    push gs

    ; 发送EOI消息，否则下一次中断不发生
    mov al, 0x20
    out 0x20, al
    out 0xa0, al
    
    call c_time_interrupt_handler

    pop gs
    pop fs
    pop es
    pop ds
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