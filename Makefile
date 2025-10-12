
OUTPUT=out/kernel
ISOOUTPUT=out/os.iso
HDDOUTPUT=out/hdd.img

AS=nasm
CC=gcc
CXX=i686-elf-g++
LD=$(CXX)

ASFLAGS=-f elf
CXXFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Iincl
LDFLAGS=-T link.ld -o $(OUTPUT) -ffreestanding -O2 -nostdlib -lgcc

ASFILES=$(shell find src/kernel -type f -name *.asm)
CXXFILES=$(shell find src/kernel -type f -name *.cpp)
CXXOBJ=$(CXXFILES:src/%.cpp=obj/%.o)
ASOBJ=$(ASFILES:src/%.asm=obj/%.o)

all: clean bootloader $(ASOBJ) $(CXXOBJ) $(OUTPUT) iso-grub img

bootloader:
	$(AS) src/boot/legacy/stage1.asm -f bin -o out/stage1.bin
	$(AS) src/boot/legacy/nosokldr.asm -f bin -o out/nosokldr.bin

obj/%.o: src/%.asm
	$(AS) $< $(ASFLAGS) -o obj/$(notdir $@)

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o obj/$(notdir $@)

$(OUTPUT):
	$(LD) $(LDFLAGS) obj/*.o

clean:
	rm -f obj/*.o
	rm -f out/*.bin
	rm -f out/*.img
	rm -f out/*.iso
	rm -rf iso

img:
	dd if=/dev/zero of=$(HDDOUTPUT) bs=512 count=65536

	mkfs.fat -F 16 $(HDDOUTPUT)

	dd if=out/stage1.bin of=$(HDDOUTPUT) bs=512 count=1 conv=notrunc

	mcopy -i $(HDDOUTPUT) out/nosokldr.bin ::/nosokldr.bin
	mcopy -i $(HDDOUTPUT) $(OUTPUT) ::kernel

iso-grub:
	mkdir -p iso/boot/grub

	cp $(OUTPUT) iso/boot/kernel

	echo 'menuentry "nOSok" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/kernel' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg

	grub-mkrescue --output $(ISOOUTPUT) iso -- -hfsplus off
	rm -rf iso

run:
	qemu-system-i386 -kernel $(OUTPUT) -d int --no-reboot

run-iso:
	qemu-system-i386 -cdrom $(ISOOUTPUT) -d int --no-reboot

run-gdb:
	qemu-system-i386 -kernel $(OUTPUT) -d int --no-reboot -s -S

run-img:
	qemu-system-i386 -hdd out/hdd.img

install-dependencies:
	sudo apt install grub-common
	sudo apt install grub-pc-bin
	sudo apt install grub-efi-amd64-bin
