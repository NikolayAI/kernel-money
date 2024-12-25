# kernel-money

Dependencies:

- An x86 computer (of course)
- Linux
- NASM assembler
- gcc
- ld (GNU Linker)
- grub

```
nasm -f elf32 kernel.asm -o kasm.o
gcc -m32 -c kernel.c -o kc.o
ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o
qemu-system-i386 -kernel kernel -machine pcspk-audiodev=spk -audiodev id=spk,driver=pa
```