# Operating System

> [!CAUTION]
> **Do Not Install on Your Main System**
> 
> **This operating system is under development and not suitable for general use.** Installing it on your primary system could potentially lead to data loss or system instability.
> 
> **For testing purposes only, please use a virtual machine like VirtualBox or VMware.**

![Operating System](./os.png)

A 32-bit Operating System written from scratch in C++

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

> [!TIP]
> At this point, you can use a Lightweight Operating System like TinyCore Linux, to prepare the disk for testing :
> - Use `sudo fdisk <disk ex: /dev/sda>` to make a partition table
> - Use a tool like `mkfs.vfat` to make a **FAT32** file-system
> - Mount the disk, and do `cd /mnt/sda1`
> - Now you can create 2-3 files to try read from
> - It will be a while before we create a **Directory Parser**, since we'll need to create a standard library using dynamic allocation before we get to 