# # 定义常量，包括编译器、汇编器、编译选项、链接器等
# ASM_COMPILER = nasm
# C_COMPLIER = gcc
# CXX_COMPLIER = g++
# CXX_COMPLIER_FLAGS = -g -Wall -march=i386 -m32 -nostdlib -fno-builtin -ffreestanding -fno-pic
# LINKER = ld

# # 定义文件目录
# SRCDIR = ../src
# RUNDIR = ../run
# BUILDDIR = .
# INCLUDE_PATH = ../include

# # 定义文件位置，wildcard获取指定文件名的文件列表
# CXX_SOURCE += $(wildcard $(SRCDIR)/kernel/*.cpp)
# CXX_OBJ += $(CXX_SOURCE:$(SRCDIR)/kernel/%.cpp=%.o)

# ASM_SOURCE += $(wildcard $(SRCDIR)/utils/*.asm)
# ASM_OBJ += $(ASM_SOURCE:$(SRCDIR)/utils/%.asm=%.o)

# OBJ += $(CXX_OBJ)
# OBJ += $(ASM_OBJ)

# create_vhd:
# 	qemu-img create vhd.img 10m

# # 将二进制文件写入硬盘
# build: mbr.bin bootloader.bin kernel.bin
# 	dd if=mbr.bin of=$(RUNDIR)/vhd.img bs=512 count=1 seek=0 conv=notrunc
# 	dd if=bootloader.bin of=$(RUNDIR)/vhd.img bs=512 count=5 seek=1 conv=notrunc
# 	dd if=kernel.bin of=$(RUNDIR)/vhd.img bs=512 count=145 seek=6 conv=notrunc

# mbr.bin : $(SRCDIR)/boot/mbr.asm
# 	$(ASM_COMPILER) -o mbr.bin -f bin -I$(INCLUDE_PATH)/ $(SRCDIR)/boot/mbr.asm

# bootloader.bin : $(SRCDIR)/boot/bootloader.asm
# 	$(ASM_COMPILER) -o bootloader.bin -f bin -I$(INCLUDE_PATH)/ $(SRCDIR)/boot/bootloader.asm

# entry.obj: $(SRCDIR)/boot/entry.asm
# 	$(ASM_COMPILER) -o $@ -f elf32 $^
# # objcopy命令将文件以一定格式拷贝到另一个文件中
# kernel.bin : kernel.o
# 	objcopy -O binary kernel.o kernel.bin

# # -N：指定读取/写入文本和数据段 -e：使用指定的符号作为程序的初始执行点
# kernel.o : entry.obj $(OBJ)
# 	$(LINKER) -o kernel.o -melf_i386 -N entry.obj $(OBJ) -e enter_kernel -Ttext 0x0002000

# $(CXX_OBJ):
# 	$(CXX_COMPLIER) $(CXX_COMPLIER_FLAGS) -I$(INCLUDE_PATH) -c $(CXX_SOURCE)

# asm_utils.o: $(SRCDIR)/utils/asm_utils.asm
# 	$(ASM_COMPILER) -o asm_utils.o -f elf32 $(SRCDIR)/utils/asm_utils.asm

# clean:
# 	rm -f *.o* *.bin

# run:
# 	qemu-system-i386 -hda $(RUNDIR)/vhd.img -serial null -parallel stdio -no-reboot

# debug:
# 	qemu-system-i386 -S -s -parallel stdio -hda $(RUNDIR)/vhd.img -serial null&
# 	@sleep 1
# 	gnome-terminal -e "gdb -q -tui -x $(RUNDIR)/gdbinit"

ASM_COMPILER = nasm
C_COMPLIER = gcc
CXX_COMPLIER = g++
CXX_COMPLIER_FLAGS = -g -Wall -march=i386 -m32 -nostdlib -fno-builtin -ffreestanding -fno-pic
LINKER = ld

SRCDIR = ../src
RUNDIR = ../run
BUILDDIR = build
INCLUDE_PATH = ../include

CXX_SOURCE += $(wildcard $(SRCDIR)/kernel/*.cpp)
CXX_OBJ += $(CXX_SOURCE:$(SRCDIR)/kernel/%.cpp=%.o)

ASM_SOURCE += $(wildcard $(SRCDIR)/utils/*.asm)
ASM_OBJ += $(ASM_SOURCE:$(SRCDIR)/utils/%.asm=%.o)

OBJ += $(CXX_OBJ)
OBJ += $(ASM_OBJ)

build : mbr.bin bootloader.bin kernel.bin kernel.o
	dd if=mbr.bin of=$(RUNDIR)/hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=$(RUNDIR)/hd.img bs=512 count=5 seek=1 conv=notrunc
	dd if=kernel.bin of=$(RUNDIR)/hd.img bs=512 count=145 seek=6 conv=notrunc
# nasm的include path有一个尾随/

mbr.bin : $(SRCDIR)/boot/mbr.asm
	$(ASM_COMPILER) -o mbr.bin -f bin -I$(INCLUDE_PATH)/ $(SRCDIR)/boot/mbr.asm
	
bootloader.bin : $(SRCDIR)/boot/bootloader.asm 
	$(ASM_COMPILER) -o bootloader.bin -f bin -I$(INCLUDE_PATH)/ $(SRCDIR)/boot/bootloader.asm
	
entry.obj : $(SRCDIR)/boot/entry.asm
	$(ASM_COMPILER) -o entry.obj -f elf32 $(SRCDIR)/boot/entry.asm
	
kernel.bin : kernel.o
	objcopy -O binary kernel.o kernel.bin
	
kernel.o : entry.obj $(OBJ)
	$(LINKER) -o kernel.o -melf_i386 -N entry.obj $(OBJ) -e enter_kernel -Ttext 0x00020000
	
$(CXX_OBJ):
	$(CXX_COMPLIER) $(CXX_COMPLIER_FLAGS) -I$(INCLUDE_PATH) -c $(CXX_SOURCE)
	
asm_utils.o : $(SRCDIR)/utils/asm_utils.asm
	$(ASM_COMPILER) -o asm_utils.o -f elf32 $(SRCDIR)/utils/asm_utils.asm
clean:
	rm -f *.o* *.bin 
	
run:
	qemu-system-i386 -hda $(RUNDIR)/hd.img -serial null -parallel stdio -no-reboot

debug: 
	qemu-system-i386 -S -s -parallel stdio -hda $(RUNDIR)/hd.img -serial null&
	@sleep 1
	gnome-terminal -e "gdb -q -tui -x $(RUNDIR)/gdbinit"
