 SRC = mbr.asm


.PHONY: clean, run, build

# -f制定输出文件格式  bin保存的是机器可以识别的机器指令，通过命令xxd查看其中的内容
# 将mbr写入hd.img的首扇区
# if表示输入文件
# of表示输出文件
# bs表示块大小
# count表示写入块数目
# seek表示越过输出文件多少块后再写入
# conv=notrunc表示传输完后不截断磁盘
build:
	nasm -g -f elf32 $(SRC) -o mbr.o
	ld -o mbr.symbol -melf_i386 -N mbr.o -Ttext 0x7c00
	ld -o mbr.bin -melf_i386 -N mbr.o -Ttext 0x7c00 --oformat binary
	dd if=mbr.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc

# 打开qemu并阻塞CPU执行，执行后sleep 1给qemu足够的启动时间
# -hda表示将指定文件作为第0号磁盘映像
#  -serial 表示重定向虚拟串口
#  -parallel表示重定向虚拟并口到指定设备
# 启动gdb，这里gonme-terminal表示开启一个终端，-e参数指定开启终端后执行的命令，如果命令中有空格要加引号
# gdb命令：-q指定不打印一些版本信息，-x参数指定执行后面文件中的命令，里面是连接上qemu的一些命令
debug:
	qemu-system-i386 -s -S -hda hd.img -serial null -parallel stdio & sleep 1
	gnome-terminal -e "gdb -q -x gdbinit"

create_vhd:
	qemu-img create hd.img 10m

run:
	qemu-system-i386 -hda hd.img -serial null -parallel stdio

clean:
	rm -rf *.bin *.o
