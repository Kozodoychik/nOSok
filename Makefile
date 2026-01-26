
OUTPUT=out/kernel
ISOOUTPUT=out/os.iso
HDDOUTPUT=out/hdd.img

AS=nasm
CC=i686-elf-gcc
CXX=i686-elf-g++
LD=$(CC)

ASFLAGS=-f elf
CXXFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Iincl
CFLAGS=-ffreestanding -O2 -Wall -Wextra
LDFLAGS=-T link.ld -o $(OUTPUT) -ffreestanding -O2 -nostdlib -lgcc

ASFILES=$(shell find src/kernel -type f -name *.asm)
CXXFILES=$(shell find src/kernel -type f -name *.cpp)
CXXOBJ=$(CXXFILES:src/kernel/%.cpp=obj/kernel/%.o)
ASOBJ=$(ASFILES:src/kernel/%.asm=obj/kernel/%.o)

all: clean bootloader $(ASOBJ) $(CXXOBJ) $(OUTPUT) img

bootloader:
	$(AS) src/boot/legacy/stage1.asm -f bin -o out/stage1.bin
	$(AS) src/boot/legacy/nosokldr.asm -f elf -o obj/nosokldr_asm.o
	$(CC) $(CFLAGS) -c src/boot/legacy/nosokldr.c -o obj/nosokldr_c.o
	$(CC) -T src/boot/legacy/link.ld -ffreestanding -O2 -nostdlib -o out/nosokldr.bin obj/nosokldr_asm.o obj/nosokldr_c.o

obj/kernel/%.o: src/kernel/%.asm
	$(AS) $< $(ASFLAGS) -o obj/kernel/$(notdir $@)

obj/kernel/%.o: src/kernel/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o obj/kernel/$(notdir $@)

$(OUTPUT):
	$(LD) $(LDFLAGS) obj/kernel/*.o

clean:
	rm -f obj/*.o
	rm -f obj/kernel/*.o
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

run:
	qemu-system-i386 -kernel $(OUTPUT) -d int --no-reboot

run-gdb:
	qemu-system-i386 -kernel $(OUTPUT) -d int --no-reboot -s -S

run-img:
	qemu-system-i386 -hdd out/hdd.img
