SRC = print_id

.PHONY: clean, run, build

build:
	nasm -f bin $(SRC).asm -o $(SRC).bin
	dd if=$(SRC).bin of=hd.img bs=512 count=1 seek=0 conv=notrunc

create_vhd:
	qemu-img create hd.img 10m

run:
	qemu-system-i386 -hda hd.img -serial null -parallel stdio

clean:
	rm -rf *.bin *.o
