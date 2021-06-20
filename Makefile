ARCH = x86

BOOT_BIN_NAME = NOVALDR.BIN
BOOT_BIN = boot.bin
BOOT2_BIN = boot2.bin

BOOT_STAGES := $(wildcard src/arch/$(ARCH)/*asm)
BOOT_BINARIES := $(patsubst src/arch/$(ARCH)%, out/$(ARCH)%, $(patsubst %.asm, %.bin, $(BOOT_STAGES)))

BOOT_IMG = out/$(ARCH)/boot.img

QEMU_ARGS = -drive format=raw,file=$(BOOT_IMG)
BOOT_INCLUDES := $(wildcard src/arch/$(ARCH)/16bit/*.asm)
MEM_FILES := $(wildcard *.mem)
DUMP_FILES := $(patsubst %.mem, %.dump, $(MEM_FILES))

KERNEL_SRC := $(shell find src/kernel -name '*.cpp')
KERNEL_OBJ := $(patsubst src/kernel%, out/$(ARCH)/kernel%, $(patsubst %.cpp, %.o, $(KERNEL_SRC)))

# INCLUDES := $(shell find include -name '*.h')

test:
	echo $(KERNEL_SRC)
	echo $(KERNEL_OBJ)

USB_PATH = /dev/sdb

out/$(ARCH)/%.bin: src/arch/x86/%.asm
	mkdir -p $(@D)
	nasm -f bin $^ -o $@ -i src/arch/$(ARCH)

out/$(ARCH)/kernel/%.o: src/kernel/%.cpp
	mkdir -p $(@D)
	i686-elf-g++ -c $^ -o $@ -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Iinclude

%.dump: %.mem
	xxd $^ > $@

run:
	make boot
	make xxd

out/$(ARCH)/kernel.img: $(KERNEL_OBJ)
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc -Iinclude

boot: $(BOOT_IMG)
ifeq ($(ARCH), x86)
	qemu-system-i386 $(QEMU_ARGS)
endif
ifeq ($(ARCH), x86_64)
	qemu-system-x86_64 $(QEMU_ARGS)
endif
ifeq ($(ARCH), arm)
	qemu-system-arm $(QEMU_ARGS)
endif

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
