/* Declare constants for the multiboot header. */
.set ALIGN,	1<<0		/* align loaded modules on page boundaries */
.set MEMINFO,	1<<1		/* provide memory map */
.set FLAGS,	ALIGN | MEMINFO /* this is the Multiboot 'flag' field */
.set MAGIC,	0x1BADB002	/* 'magic number' lets bootloader find the header */
.set CHECKSUM,  -(MAGIC + FLAGS)/* checksum of above, to prove we are multiboot */

/*
	declare a multiboot header that marks the program as a kernel. These are magic
	values that are documented in the multiboot standard. The bootloader will
	search for this signature in the first 8KiB of the kernel file, aligned at a
	32-bit boundary. The signature is in its own section so the header can be
	forced to be within the first 8 KiB of the kernel file.
*/

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/*
	the multiboot standard does not define the value of the stack pointer register
	(esp) and it is up to the kernel to provide a stack. This allocates room for a
	small stack by creating a stimbol at the bottom of it, then allocating 16384
	bytes for it, and finally creating a symbol at the top. The stack grows
	downwards on x86. The stack is in its own section so it can be marked nobits,
	which means the kernel file is smaller because it does not contain an
	uninitialized stack. The stack on x86 must be 16-byte aligned accoring to the
	System V ABI standard and de-facto extensions. The compiler will assume the
	stack is properly aligned and failure to align the stack will result in
	undefined behaviour.
*/

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

/*
	the linker script specifies _start as the entry point to the kernel and the
	bootloader will jump to this position once the kernel has been loaded. It
	dosen't make sense to return from this function as the bootloader is gone.
*/

.section .text
.global _start
.type _start, @function

_start:

/*
	IMPORTANT:
*/

/*
	the bootloader has loaded us into 32-bit protected modde on a x86 machine.
	Interrupts are disabled. Paging is disabled. The processor state is as
	defined in the multiboot standard. The kernel has full control of the CPU.
	The kernel can only make use of hardware deatures and any code it provides
	as part of itself. There's no printf(), unless the kernel provides the
	<stdio.h> header and a printf() implementation. There are no security
	restrictions, no safeguards, no debugging mechanisms, only what the kernel
	provides itself. It has absolute and complete power over the machine.
*/

/*
	Setting up a stack:

	To set up a stack, we set the esp register to point to the top of the
	stack (as it grows downards on x86 systems). This is necessarily done in
	assembly languages such as C cannot function without a stack.
*/

	mov $stack_top, %esp

/*
	This is a good place to initialize crucial processor state before the
	high-level kernel is entered. It's best to minimze the early environment
	where crucial features are offline. Note that the processor is not fully
	initialized yet: Features such as floading point instructions and instruction
	set extensions are not initialized yet. The GDT should be loaded here. Paging
	should be enabled here. C++ features such as constructors and exceptions will
	require runtime support to work as well.
*/

/*
	Enter the high-level kernel. THE ABI requires the stack is 16-byte aligned at
	te time of the call instruction (which afterwards pushes the return pointer of
	size 4 bytes). The stack was originally 16-byte aligned aboce and we've pushed
	a multiple of 16 bytes to the stack since (pushed 0 bytes so far), so the
	alignment has thus been preserved and the call is well defined
*/

	call kernel_main

/*
	If the system has nothing more to do, put the computer into an infinite loop.
	To do that:

	1) Disable interrupts with cli. They are already disabled by the bootloader.
	You might later enable interrupts though.
	2) Wait for the next interrupt to arrive with hlt (halt process/instruction).
	Since this is disabled, this will lockup the computer.
	3) Jump to the hlt instruction if it ever wakes up due to a non-maskable
	interrupt occuring or due to system managment mode.
*/
	cli
1:	hlt
	jmp 1b

/*
	Set the size of the _start symbol to the current location ('.') minus it's
	start. This is useful when debugging or when you implement call tracing.
*/
.size _start, . - _start
