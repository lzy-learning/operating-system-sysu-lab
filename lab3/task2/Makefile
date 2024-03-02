create_vhd:
	qemu-img create vhd.img 10m
build:
	nasm -f bin -o mbr.bin mbr.asm
	nasm bootloader.asm -f bin -o bootloader.bin
	dd if=mbr.bin of=vhd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=vhd.img bs=512 count=5 seek=1 conv=notrunc


run:
	qemu-system-i386 -hda vhd.img -serial null -parallel stdio
clean:
	rm -rf *.bin *.o *.symbol
