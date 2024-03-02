create_vhd:
	qemu-img create vhd.img 10m
build:
	nasm -o mbr.o -g -f elf32 mbr.asm
	ld -o mbr.symbol -melf_i386 -N mbr.o -Ttext 0x7c00
	ld -o mbr.bin -melf_i386 -N mbr.o -Ttext 0x7c00 --oformat binary
	nasm -o bootloader.o -g -f elf32 bootloader.asm
	ld -o bootloader.symbol -melf_i386 -N bootloader.o -Ttext 0x7e00
	ld -o bootloader.bin -melf_i386 -N bootloader.o -Ttext 0x7e00 --oformat binary
	dd if=mbr.bin of=vhd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=vhd.img bs=512 count=5 seek=1 conv=notrunc

debug:
	qemu-system-i386 -s -S -hda vhd.img -serial null -parallel stdio & 
	sleep 1
	gnome-terminal -e "gdb -q -x gdbinit"

run:
	qemu-system-i386 -hda vhd.img -serial null -parallel stdio
clean:
	rm -rf *.bin *.o *.symbol
