; 加载器扇区数
LOADER_SECTOR_COUNT equ 5
; 加载器起始扇区
LOADER_START_SECTOR equ 1
; 加载器被加载地址
LOADER_START_ADDRESS equ 0x7e00

; GDT起始位置
GDT_START_ADDRESS equ 0x8800

;平坦模式数据段选择子
DATA_SELECTOR equ 0x8
;平坦模式栈段选择子
STACK_SELECTOR equ 0x10
;平坦模式视频段选择子
VIDEO_SELECTOR equ 0x18
VIDEO_NUM equ 0x18
;平坦模式代码段选择子
CODE_SELECTOR equ 0x20

; __________kernel_________
; 内核在磁盘中的起始扇区
KERNEL_START_SECTOR equ 6
; 内核所占扇区数
KERNEL_SECTOR_COUNT equ 200
; 内核在内存中的起始地址
KERNEL_START_ADDRESS equ 0x20000