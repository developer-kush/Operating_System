.set MAGICNUM, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGICNUM + FLAGS)

.section .multiboot
    .long MAGICNUM
    .long FLAGS
    .long CHECKSUM

.section .text
.extern myKernel
.global loader

loader: 
    mov $kernel_stack, %esp

    call callConstructors

    push %eax
    push %ebx
    call myKernel

_stop:
    cli
    hlt 
    jmp _stop

.section .bss
.space 2*1024*1024; # 2 MiB
kernel_stack: