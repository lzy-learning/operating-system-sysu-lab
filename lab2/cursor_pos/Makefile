SRC = cursor_pos

.PHONY: clean, run, build

build:
	nasm -f elf32 $(SRC).asm -o $(SRC).o
	ld -o $(SRC).symbol -melf_i386 -N $(SRC).o -Ttext 0x7c00
	ld -o $(SRC).bin -melf_i386 -N $(SRC).o -Ttext 0x7c00 --oformat binary
	dd if=$(SRC).bin of=hd.img bs=512 count=1 seek=0 conv=notrunc

create_vhd:
	qemu-img create hd.img 10m

run:
	qemu-system-i386 -hda hd.img -serial null -parallel stdio

clean:
	rm -rf *.bin *.o
