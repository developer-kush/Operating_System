# Operating System

![Operating System](./os.png)

A 32-bit Operating System written from scratch in C++
 
> This project is in development
> This Operating system is not meant for general usage, as this is a very bare-bones Operating System and made only for learning and educational purposes

# Status

In the current status, this Operating System can only handle user inputs to type and mouse movements

# Installation

In order to build this from source ...

1. Install dependencies

```bash
sudo apt-get install g++ binutils libc6-dev-i386
sudo apt-get install grub-legacy xorriso
```

2. Build the ISO image

```bash
make mykernel.iso
```

3. Run on a Virtual Machine using VirtualBox or VM Ware