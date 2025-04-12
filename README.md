# AmberOS

AmberOS is an operating system made solo by me!

So far, it has:

- Newline character function ('\n');
- Terminal scrolling;

## How to compile the binary and .iso files? (linux)

To compile these files, you will need the following packages (linux):

- grub-common
- grub-mkrescue
- grub-efi-amd64 (if you are on a UEFI system)
- qemu-system-common
- build-essentials (required for use of Make and to compile c files.)
- binutils

Additionally, you will need a cross compiler if you would like to
compile the assembly/c files. A link can be found [here](https://wiki.osdev.org/GCC_Cross-Compiler)

I will leave the object files unless I need to remove them.

Inside bash, you need to set these 3 variables

'export PREFIX="$HOME/opt/cross"'
'export TARGET=i686-elf'
'export PATH="$PREFIX/bin:$PATH"

Run these commands in order

- make binary
- make iso-file
- make qemu

### Credits

Mainly, I shall thank [OsDev](https://wiki.osdev.org) for helping me with these tutorials and helping me understand the general idea of Operating Systems.