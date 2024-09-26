
GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = --32
LDPARAMS = -melf_i386

OBJECTS = loader.o kernel.o gdt.o port.o interrupts.o interruptstubs.o

%.o: %.cpp
		g++ $(GPPPARAMS) -o $@ -c $<

%.o: %.s
		as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(OBJECTS)
		ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)

install: mykernel.bin
		sudo cp $< /boot/mykernel.bin

mykernel.iso: mykernel.bin
		mkdir iso
		mkdir iso/boot
		mkdir iso/boot/grub
		cp $< iso/boot/
		echo 'set timeout=0' > iso/boot/grub/grub.cfg
		echo 'set default=0' >> iso/boot/grub/grub.cfg
		echo '' >> iso/boot/grub/grub.cfg
		echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
		echo '	multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
		echo '	boot' >> iso/boot/grub/grub.cfg
		echo '}' >> iso/boot/grub/grub.cfg
		grub-mkrescue --output=$@ iso

# run: mykernel.iso
# 		(killall VirtualBox && sleep 1) || true
# 		VirtualBox --startvm "My Operating System"

run: mykernel.bin
		qemu-system-i386 -kernel mykernel.bin

.PHONY: clean
clean:
		rm -rf $(OBJECTS) mykernel.bin iso