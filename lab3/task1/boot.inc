; 定义常量，主要是内存规划相关常数
; 使用equ伪指令，编译器会将LOADER_SECTOR_COUNT的地方替换成5

; 加载器扇区数
LOADER_SECTOR_COUNT equ 5
; bootloader起始扇区
LOADER_START_SECTOR equ 1
; bootloader被加载地址
LOADER_START_ADDRESS equ 0x7e00

; GDT起始位置
GDT_START_ADDRESS equ 0x8800
