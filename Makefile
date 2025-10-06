
OUTPUT=out/kernel
ISOOUTPUT=out/os.iso

AS=nasm
CXX=i686-elf-g++
LD=$(CXX)

ASFLAGS=-f elf
CXXFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Iincl
LDFLAGS=-T link.ld -o $(OUTPUT) -ffreestanding -O2 -nostdlib -lgcc

ASFILES=$(shell find src -type f -name *.asm)
CXXFILES=$(shell find src -type f -name *.cpp)
CXXOBJ=$(CXXFILES:src/%.cpp=obj/%.o)
ASOBJ=$(ASFILES:src/%.asm=obj/%.o)

all: clean $(ASOBJ) $(CXXOBJ) $(OUTPUT) iso

obj/%.o: src/%.asm
	$(AS) $< $(ASFLAGS) -o obj/$(notdir $@)

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o obj/$(notdir $@)

$(OUTPUT):
	$(LD) $(LDFLAGS) obj/*.o

clean:
	rm -f obj/*
	rm -f out/*
	rm -rf iso

iso:
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	
	cp $(OUTPUT) iso/boot/kernel
	
	echo 'menuentry "nOSok" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/kernel' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg

	grub-mkrescue --output $(ISOOUTPUT) iso
	rm -rf iso

run:
	qemu-system-i386 -kernel $(OUTPUT)

run-iso:
	qemu-system-i386 -cdrom $(ISOOUTPUT)