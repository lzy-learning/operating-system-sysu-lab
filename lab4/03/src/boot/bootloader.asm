org 0x7e00
%include "boot.inc"
[bits 16]
;空描述符
mov dword [GDT_START_ADDRESS+0x00],0x00
mov dword [GDT_START_ADDRESS+0x04],0x00  

;创建描述符，这是一个数据段，对应0~4GB的线性地址空间
mov dword [GDT_START_ADDRESS+0x08],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x0c],0x00cf9200    ; 粒度为4KB，存储器段描述符 

;建立保护模式下的堆栈段描述符      
mov dword [GDT_START_ADDRESS+0x10],0x00000000    ; 基地址为0x00000000，界限0x0 
mov dword [GDT_START_ADDRESS+0x14],0x00409600    ; 粒度为1个字节

;建立保护模式下的显存描述符   
mov dword [GDT_START_ADDRESS+0x18],0x80007fff    ; 基地址为0x000B8000，界限0x07FFF 
mov dword [GDT_START_ADDRESS+0x1c],0x0040920b    ; 粒度为字节

;创建保护模式下平坦模式代码段描述符
mov dword [GDT_START_ADDRESS+0x20],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x24],0x00cf9800    ; 粒度为4kb，代码段描述符 

;初始化描述符表寄存器GDTR
mov word [pgdt], 39      ;描述符表的界限   
lgdt [pgdt]
      
in al,0x92                         ;南桥芯片内的端口 
or al,0000_0010B
out 0x92,al                        ;打开A20

cli                                ;中断机制尚未工作
mov eax,cr0
or eax,1
mov cr0,eax                        ;设置PE位

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

; 内核起始扇区
mov eax, KERNEL_START_SECTOR
; 内核载入地址
mov ebx, KERNEL_START_ADDRESS
; 内核扇区数
mov ecx, KERNEL_SECTOR_COUNT

; 加载内核
load_kernel:
    push eax
    push ebx
    call asm_read_hard_disk
    pop ebx
    pop eax
    inc eax
    add ebx, 512
    loop load_kernel

jmp dword CODE_SELECTOR:KERNEL_START_ADDRESS
jmp $

; 读取一个扇区大小的内核到内存中
asm_read_hard_disk:
    push ebp
    mov ebp, esp

    ; 这里可以压入eax，ebx也可以不保存
    push ecx
    push edx
    
    mov eax, [ebp+4*3]  ; 注意在保护模式下，压入栈的数据都是32位的了，所以乘4

    ; 将28位逻辑扇区号传入相应端口，并设置为LBA模式从主硬盘读
    mov edx, 0x1f3
    out dx, al
    inc edx
    mov al, ah
    out dx, al
    xor eax, eax
    inc edx
    out dx, al
    inc edx
    mov al, ah
    and al, 0x0f
    or al, 0xe0
    out dx, al
    ; 读取扇区数
    mov edx, 0x1f2
    mov al, 1
    out dx, al
    ; 发送读取命令
    mov edx, 0x1f7
    mov al, 0x20
    out dx, al
    .waits:
        in al, dx
        and al, 0x88
        cmp al, 0x08
        jnz .waits
    mov ebx, [ebp+4*2]
    mov ecx, 256
    mov edx, 0x1f0
    .readw:
        in ax, dx
        mov [ebx], eax
        add ebx, 2
        loop .readw
    pop edx
    pop ecx
    pop ebp
    ret

; 描述符表的内存地址
pgdt dw 0 
    dd GDT_START_ADDRESS