binary:
	i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib assembly/boot.o c/kernel/kernel.c -lgcc

iso-file:
	grub-mkrescue -o myos.iso isodir

kernel-c:
	i686-elf-gcc -c c/kernel/kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

qemu:
	qemu-system-i386 -kernel myos.bin