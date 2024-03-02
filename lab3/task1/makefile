create_vhd:
	qemu-img create vhd.img 10m

# -f bin指定输出的文件格式
build:
	nasm -f bin bootloader.asm -o bootloader.bin
	nasm -f bin mbr.asm -o mbr.bin
	dd if=mbr.bin of=vhd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=vhd.img bs=512 count=5 seek=1 conv=notrunc

run:
	qemu-system-i386 -hda vhd.img -serial null -parallel stdio

clean:
	rm -rf *.bin *.o

