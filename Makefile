all: dingusOS.bin

boot.o:  boot.s
	@echo "building..."
	i686-elf-as boot.s -o bin/boot.o

kernel.o: kernel.c
	i686-elf-gcc -c kernel.c -o bin/kernel.o -std=gnu99 -ffreestanding -O2 -Wall

dingusOS.bin: boot.o kernel.o linker.ld
	i686-elf-gcc -T linker.ld -o bin/dingusOS.bin -ffreestanding -O2 -nostdlib bin/boot.o bin/kernel.o -lgcc

clean:
	@echo "Removing build files..."
	rm bin/*.o bin/dingusOS.bin