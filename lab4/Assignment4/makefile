# 内核编译全过程
# 编译mbr和bootloader，直接编译为二进制文件，因为它们不需要链接
nasm -o mbr.bin -g -f bin -I./include ./src/boot/mbr.asm
nasm -o bootloader.bin -g -f bin -I./include ./src/boot/bootloader

# 编译内核入口
# 关于bootloader为什么不需要链接entry.obj，因为bootloader通过远跳转到内存的某个位置进入内核，而不是调用函数
nasm -o entry.obj -g -f elf32 ./src/boot/entry.asm

# 编译内核文件
g++ -o setup.o -g -Wall -march=i386 -m32 -nostdlib -fno=builtin -ffreestanding -fno-pic -I./include -c ./src/kernel/setup.cpp
g++ -o stdio.o -g -Wall -march=i386 -m32 -nostdlib -fno=builtin -ffreestanding -fno-pic -I./include -c ./src/kernel/stdio.cpp
g++ -o interrupt.o -g -Wall -march=i386 -m32 -nostdlib -fno=builtin -ffreestanding -fno-pic -I./include -c ./src/kernel/interrupt.cpp
g++ -o marquee.o -g -Wall -march=i386 -m32 -nostdlib -fno=builtin -ffreestanding -fno-pic -I./include -c ./src/kernel/marquee.cpp
# 编译工具函数文件
nasm -o asm_utils.o -f elf32 -I./include ./src/utils/asm_utils.asm
# 链接内核文件
ld -o kernel.o -melf_i386 -N entry.obj setup.o stdio.o interrupt.o marquee.o -e enter_kernel -Ttext 0x00020000
# 提取代码部分并转化为二进制文件
objcopy -O binary kernel.o kernel.bin

# 写入虚拟磁盘
dd if=mbr.bin of=./run/hd.img bs=512 count=1 seek=0 conv=notrunc
dd if=bootloader.bin of=./run/hd.img bs=512 count=5 seek=1 conv=notrunc
dd if=kernel.bin of=./run/hd.img bs=512 count=145 seek=6 conv=notrunc


