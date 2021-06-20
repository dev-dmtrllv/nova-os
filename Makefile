ARCH = x86
BOOT_ARCH := $(ARCH)

CFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Iinclude
QEMU_FLAGS = -drive format=raw,file=$(BOOT_IMG)

BOOT_BIN_NAME = NOVALDR.BIN
BOOT_BIN = boot.bin
BOOT2_BIN = boot2.bin

BOOT_STAGES := $(wildcard src/arch/$(BOOT_ARCH)/boot/*asm)
BOOT_BINARIES := $(patsubst src/arch/$(BOOT_ARCH)/boot%, out/$(BOOT_ARCH)%, $(patsubst %.asm, %.bin, $(BOOT_STAGES)))

BOOT_IMG = out/$(BOOT_ARCH)/boot.img

# BOOT_x86_INCLUDES := $(wildcard src/arch/$(ARCH)/boot/16bit/*.asm)
MEM_FILES := $(wildcard *.mem)
DUMP_FILES := $(patsubst %.mem, %.dump, $(MEM_FILES))

KERNEL_SRC := $(shell find src/kernel -name '*.cpp')
KERNEL_OBJ := $(patsubst src/kernel%, out/$(ARCH)/kernel%, $(patsubst %.cpp, %.o, $(KERNEL_SRC)))
KERNEL_INCLUDES := $(shell find include -name '*.h')

USB_PATH = /dev/sdb

ifeq ($(ARCH), x86)
	GCC = i686-elf-g++
	LD = i686-elf-gcc
	QEMU = qemu-system-i386
	BOOT_ARCH = $(ARCH)
else ifeq ($(ARCH), x86_64)
	GCC = x86_64-elf-g++
	LD = x86_64-elf-gcc
	QEMU = qemu-system-x86_64
	BOOT_ARCH = x86
	CFLAGS += -mno-red-zone
else ifeq ($(ARCH), arm)
	GCC = x86_64-elf-g++
	LD = x86_64-elf-gcc
	QEMU = qemu-system-arm
	BOOT_ARCH = $(ARCH)
endif

# create bootloader for x86 and x86_64
out/$(BOOT_ARCH)/%.bin: src/arch/$(BOOT_ARCH)/boot/%.asm
	@mkdir -p $(@D)
	nasm -f bin $^ -o $@ -i src/arch/$(BOOT_ARCH)/boot


out/$(ARCH)/kernel/%.o: src/kernel/%.cpp
	@mkdir -p $(@D)
	$(GCC) -c $^ -o $@ $(CFLAGS)

%.dump: %.mem
	xxd $^ > $@

run: $(BOOT_IMG)
	$(QEMU) $(QEMU_FLAGS)
	make xxd

out/$(ARCH)/kernel.img: $(KERNEL_OBJ)
	$(LD) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc -Iinclude

$(BOOT_IMG): $(BOOT_BINARIES) out/$(ARCH)/kernel.img
	test -e $@ || mkfs.fat -F 16 -C $@ 256000
# copy the first 3 bytes
	dd bs=1 if=out/$(ARCH)/$(BOOT_BIN) count=3 of=$@ conv=notrunc
# skip 91 bytes from out boot.bin because this is the BPB created by the formatter
	dd bs=1 skip=62 if=out/$(ARCH)/$(BOOT_BIN) iflag=skip_bytes of=$@ seek=62 conv=notrunc
	mcopy -n -o -i $@ out/$(ARCH)/$(BOOT2_BIN) ::/$(BOOT_BIN_NAME)
	mcopy -n -o -i $@ test.txt ::/TEST.TXT
	mcopy -n -o -i $@ out/$(ARCH)/kernel.img ::/KERNEL.IMG

usb: $(BOOT_IMG)
	sudo dd if=$^ of=$(USB_PATH) bs=1M status=progress

xxd: $(DUMP_FILES)

clean:
	rm -rf out/*
	rm -rf *.dump
	rm -rf *.mem
	rm -rf dump
