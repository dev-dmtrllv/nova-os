ARCH = x86

BOOT = grub

OPTIMIZATION = -O2

GCC_PATH = ~/opt/cross/bin

CFLAGS = -ffreestanding $(OPTIMIZATION) -Wall -Wextra -fno-exceptions -fno-rtti -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-common -Iinclude -D$(ARCH)
QEMU_FLAGS = -drive format=raw,file=$(BOOT_IMG) -no-reboot -no-shutdown
LD_FLAGS = -fno-common -ffreestanding -nostdlib -lgcc $(OPTIMIZATION)

LINK_OUTPUT = bin

ifeq ($(ARCH), x86)
	OBJCPY = $(GCC_PATH)/i686-elf-objcopy
	GCC = $(GCC_PATH)/i686-elf-g++
	LD = $(GCC_PATH)/i686-elf-g++
	QEMU = qemu-system-i386
	BOOT_ARCH := x86
else ifeq ($(ARCH), x86_64)
	OBJCPY = $(GCC_PATH)/i686-elf-objcopy
	GCC = $(GCC_PATH)/x86_64-elf-g++
	LD = $(GCC_PATH)/x86_64-elf-gcc
	QEMU = qemu-system-x86_64
	BOOT_ARCH := x86
	CFLAGS += -mno-red-zone
else ifeq ($(ARCH), arm)
# TODO
	GCC = x86_64-elf-g++
	LD = x86_64-elf-gcc
	QEMU = qemu-system-arm
	BOOT_ARCH := arm
endif

BOOT_FILES := $(wildcard src/arch/$(BOOT_ARCH)/boot/16bit/*.asm)

FILES = out/$(ARCH)/boot.bin out/$(ARCH)/kernel.img out/$(ARCH)/kinit.img

BOOT_IMG = out/$(ARCH)/os.img

MEM_FILES := $(wildcard *.mem)
DUMP_FILES := $(patsubst %.mem, %.dump, $(MEM_FILES))

KINIT_CPP_SRC := $(wildcard src/arch/$(ARCH)/kinit/*.cpp)
KINIT_ASM_SRC := $(wildcard src/arch/$(ARCH)/kinit/*.asm)

KERNEL_CPP_SRC := $(wildcard src/kernel/*.cpp)
KERNEL_DRIVERS_CPP_SRC = $(shell find src/kernel/drivers -name '*.cpp')
ARCH_SRC = $(wildcard src/arch/$(ARCH)/*.asm)

LIB_SRC = $(shell find src/lib -name '*.cpp')
LIB_OBJ := $(patsubst src/lib%, out/$(ARCH)/lib%, $(patsubst %.cpp, %.o, $(LIB_SRC)))

KINIT_OBJ = $(patsubst src/arch/$(ARCH)/kinit%, out/$(ARCH)/kinit%, $(patsubst %.cpp, %.o, $(KINIT_CPP_SRC)))
KINIT_OBJ += $(patsubst src/arch/$(ARCH)/kinit%, out/$(ARCH)/kinit%, $(patsubst %.asm, %.s.o, $(KINIT_ASM_SRC)))
KINIT_OBJ += $(patsubst src/kernel/drivers/%, out/$(ARCH)/kinit/drivers/%, $(patsubst %.cpp, %.o, $(KERNEL_DRIVERS_CPP_SRC)))
KINIT_OBJ += $(patsubst src/arch/x86/%, out/$(ARCH)/%, $(patsubst %.asm, %.o, $(ARCH_SRC)))
KINIT_OBJ += $(LIB_OBJ)

KERNEL_OBJ = $(patsubst src/kernel/%, out/$(ARCH)/kernel/%, $(patsubst %.cpp, %.o, $(KERNEL_CPP_SRC)))
KERNEL_OBJ += $(patsubst src/kernel/drivers/%, out/$(ARCH)/kernel/drivers/%, $(patsubst %.cpp, %.o, $(KERNEL_DRIVERS_CPP_SRC)))
KERNEL_OBJ += $(patsubst src/arch/x86%, out/$(ARCH)%, $(patsubst %.asm, %.o, $(ARCH_SRC)))
KERNEL_OBJ += $(LIB_OBJ)


USB_PATH = /dev/sdb


out/x86/boot/boot1.bin: src/arch/x86/boot/boot.asm src/arch/x86/boot/16bit/common.asm src/arch/x86/boot/16bit/screen.asm
	@mkdir -p $(@D)
	nasm -f bin -i src/arch/x86/boot src/arch/x86/boot/boot.asm -o $@

out/x86/boot/boot2.bin: src/arch/x86/boot/boot2.asm $(BOOT_FILES)
	@mkdir -p $(@D)
	nasm -f bin -i src/arch/x86/boot src/arch/x86/boot/boot2.asm -o $@

out/x86/boot.bin: out/x86/boot/boot1.bin out/x86/boot/boot2.bin
	cat $^ > $@


out/$(ARCH)/kinit/%.s.o: src/arch/$(ARCH)/kinit/%.asm
	@mkdir -p $(@D)
	nasm -f elf32 $^ -o $@ 

out/$(ARCH)/kinit/%.o: src/arch/$(ARCH)/kinit/%.cpp
	@mkdir -p $(@D)
	$(GCC) $(CFLAGS) -DKERNEL_BOOT_$(ARCH) -c $^ -o $@

out/$(ARCH)/kinit/drivers/%.o: src/kernel/drivers/%.cpp
	@mkdir -p $(@D)
	$(GCC) $(CFLAGS) -Dx86 -DKERNEL_BOOT_$(ARCH) -c $^ -o $@

out/$(ARCH)/kinit/lib/%.o: src/lib/%.cpp
	@mkdir -p $(@D)
	$(GCC) $(CFLAGS) -Dx86 -DKERNEL_BOOT_$(ARCH) -c $^ -o $@

out/$(ARCH)/kernel/%.o: src/kernel/%.cpp
	@mkdir -p $(@D)
	$(GCC) -c $(CFLAGS) -D$(ARCH) $^ -o $@ 

out/$(ARCH)/lib/%.o: src/lib/%.cpp
	@mkdir -p $(@D)
	$(GCC) -c $(CFLAGS) -D$(ARCH) $^ -o $@ 


out/$(ARCH)/kinit.bin: $(KINIT_OBJ)
	$(LD) -Tkinit-linker.ld -o out/$(ARCH)/kinit.elf $(LD_FLAGS) $(KINIT_OBJ) -lgcc
	$(OBJCPY) --only-keep-debug out/$(ARCH)/kinit.elf out/$(ARCH)/kinit.sym
	$(OBJCPY) -O binary --strip-debug out/$(ARCH)/kinit.elf $@


out/$(ARCH)/kernel.bin: $(KERNEL_OBJ)
	$(LD) -Tkernel-linker.ld -o out/$(ARCH)/kernel.elf $(LD_FLAGS) $(KERNEL_OBJ) -lgcc
	$(OBJCPY) --only-keep-debug out/$(ARCH)/kernel.elf out/$(ARCH)/kernel.sym
	$(OBJCPY) -O binary --strip-debug out/$(ARCH)/kernel.elf $@

%.dump: %.mem
	xxd $^ > $@

run: $(BOOT_IMG)
	$(QEMU) $(QEMU_FLAGS)
	make xxd

debug: $(BOOT_IMG)
	gnome-terminal -- gdb
	$(QEMU) -s -S $(QEMU_FLAGS)

$(BOOT_IMG): out/$(ARCH)/boot.bin out/$(ARCH)/kinit.bin out/$(ARCH)/kernel.bin
	@mkdir -p $(@D)
	test -e $@ || mkfs.fat -F 16 -C $@ 256000
# copy the first 3 bytes
	dd bs=1 if=out/$(ARCH)/boot.bin count=3 of=$@ conv=notrunc
# skip 91 bytes from out boot.bin because this is the BPB created by the formatter
	dd bs=1 skip=62 if=out/$(ARCH)/boot.bin iflag=skip_bytes of=$@ seek=62 conv=notrunc
# mcopy -n -o -i $@ test.txt ::/TEST.TXT
# the kernel is split into 2 parts, the first will be landing in 32 bits and prepare its environment
# then check if 64bits is supported and jump to the 32 or 64bit kernel
	mcopy -n -o -i $@ out/$(ARCH)/kinit.bin ::/KINIT.BIN
	mcopy -n -o -i $@ out/$(ARCH)/kernel.bin ::/KERNEL.BIN


usb: $(BOOT_IMG)
# mountpoint -q $(USB_MOUNT_POINT) && sudo umount $(USB_MOUNT_POINT)
	sudo dd if=$^ of=$(USB_PATH) bs=1M status=progress

xxd: $(DUMP_FILES)

clean:
	rm -rf out/*
	rm -rf *.dump
	rm -rf *.mem
	rm -rf dump
