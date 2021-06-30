ARCH = x86

OPTIMIZATION = -O2

# CFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -Iinclude
CFLAGS = -ffreestanding $(OPTIMIZATION) -Wall -Wextra -fno-exceptions -fno-rtti -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -Iinclude
QEMU_FLAGS = -drive format=raw,file=$(BOOT_IMG)
LD_FLAGS = -ffreestanding -nostdlib -lgcc -Iinclude $(OPTIMIZATION)

ifeq ($(ARCH), x86)
	GCC = i686-elf-g++
	LD = i686-elf-gcc
	QEMU = qemu-system-i386
	BOOT_ARCH := x86
else ifeq ($(ARCH), x86_64)
	GCC = x86_64-elf-g++
	LD = x86_64-elf-gcc
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

BOOT_BIN_NAME = NOVALDR.BIN
BOOT_BIN = boot.bin
BOOT2_BIN = boot2.bin

BOOT_STAGES := $(wildcard src/arch/$(BOOT_ARCH)/boot/*asm)
BOOT_BINARIES := $(patsubst src/arch/$(BOOT_ARCH)%, out/$(ARCH)%, $(patsubst %.asm, %.bin, $(BOOT_STAGES)))

FILES = $(BOOT_BINARIES) out/$(ARCH)/kernel.img out/$(ARCH)/kinit.img

BOOT_IMG = out/$(ARCH)/boot/boot.img

MEM_FILES := $(wildcard *.mem)
DUMP_FILES := $(patsubst %.mem, %.dump, $(MEM_FILES))

LIB_SRC := $(shell find src/lib -name '*.cpp')
LIB_OBJ := $(patsubst src/lib%, out/$(ARCH)/lib%, $(patsubst %.cpp, %.o, $(LIB_SRC)))

KINIT_LIB_OBJ := $(patsubst src/lib%, out/$(ARCH)/kinit/lib%, $(patsubst %.cpp, %.o, $(LIB_SRC)))

KINIT_CPP_SRC := $(wildcard src/arch/x86/*.cpp)
KINIT_CPP_DRIVERS_SRC = $(shell find src/kernel/drivers -name '*.cpp')
KINIT_ASM_SRC := $(wildcard src/arch/x86/*.asm)
KINIT_CPP_OBJ := $(patsubst src/arch/x86%, out/$(ARCH)/kinit%, $(patsubst %.cpp, %.o, $(KINIT_CPP_SRC)))
KINIT_CPP_DRIVERS_OBJ += $(patsubst src/kernel/drivers%, out/$(ARCH)/kinit/drivers%, $(patsubst %.cpp, %.o, $(KINIT_CPP_DRIVERS_SRC)))
KINIT_ASM_OBJ := $(patsubst src/arch/x86%, out/$(ARCH)/kinit%, $(patsubst %.asm, %.o, $(KINIT_ASM_SRC)))
KINIT_OBJ := $(KINIT_ASM_OBJ) $(KINIT_CPP_OBJ) $(KINIT_CPP_DRIVERS_OBJ) $(KINIT_LIB_OBJ)

KERNEL_SRC := $(shell find src/kernel -name '*.cpp')
KERNEL_OBJ := $(patsubst src/kernel%, out/$(ARCH)/kernel%, $(patsubst %.cpp, %.o, $(KERNEL_SRC))) $(LIB_OBJ)

USB_PATH = /dev/sdb

# create bootloader for x86 and x86_64
out/$(ARCH)/boot/%.bin: src/arch/$(BOOT_ARCH)/boot/%.asm
	@mkdir -p $(@D)
	nasm -f bin -i src/arch/$(BOOT_ARCH)/boot -D$(ARCH) -DKERNEL_BOOT_$(ARCH) $^ -o $@ 


out/$(ARCH)/kinit/%.o: src/arch/x86/%.asm
	@mkdir -p $(@D)
	nasm -f elf32 -i src/arch/$(BOOT_ARCH)/boot -D$(ARCH) -DKERNEL_BOOT_$(ARCH) $^ -o $@ 

out/$(ARCH)/kinit/%.o: src/arch/x86/%.cpp
	@mkdir -p $(@D)
	i686-elf-g++ $(CFLAGS) -Dx86 -DKERNEL_BOOT_$(ARCH) -c $^ -o $@

out/$(ARCH)/kinit/drivers/%.o: src/kernel/drivers/%.cpp
	@mkdir -p $(@D)
	i686-elf-g++ $(CFLAGS) -Dx86 -DKERNEL_BOOT_$(ARCH) -c $^ -o $@

out/$(ARCH)/kinit/lib/%.o: src/lib/%.cpp
	@mkdir -p $(@D)
	i686-elf-g++ $(CFLAGS) -Dx86 -DKERNEL_BOOT_$(ARCH) -c $^ -o $@

out/$(ARCH)/kernel/%.o: src/kernel/%.cpp
	@mkdir -p $(@D)
	$(GCC) -c $(CFLAGS) -D$(ARCH) $^ -o $@ 

out/$(ARCH)/lib/%.o: src/lib/%.cpp
	@mkdir -p $(@D)
	$(GCC) -c $(CFLAGS) -D$(ARCH) $^ -o $@ 


%.dump: %.mem
	xxd $^ > $@

run: $(BOOT_IMG)
	$(QEMU) $(QEMU_FLAGS)
	make xxd



out/$(ARCH)/kernel.img: $(KERNEL_OBJ)
	$(LD) -T kernel_linker.ld $(LD_FLAGS) -D$(ARCH) $^ -o $@

# kinit must be compiled in 32 bits!!!
out/$(ARCH)/kinit.img: $(KINIT_OBJ)
	i686-elf-gcc -T kinit_linker.ld $(LD_FLAGS) -Dx86 -DKERNEL_BOOT$(ARCH) $^ -o $@ 



$(BOOT_IMG): $(FILES)
	@mkdir -p $(@D)
	test -e $@ || mkfs.fat -F 16 -C $@ 256000
# copy the first 3 bytes
	dd bs=1 if=out/$(ARCH)/boot/$(BOOT_BIN) count=3 of=$@ conv=notrunc
# skip 91 bytes from out boot.bin because this is the BPB created by the formatter
	dd bs=1 skip=62 if=out/$(ARCH)/boot/$(BOOT_BIN) iflag=skip_bytes of=$@ seek=62 conv=notrunc
	mcopy -n -o -i $@ out/$(ARCH)/boot/$(BOOT2_BIN) ::/$(BOOT_BIN_NAME)
	# mcopy -n -o -i $@ test.txt ::/TEST.TXT
# the kernel is split into 2 parts, the first will be landing in 32 bits and prepare its environment
# then check if 64bits is supported and jump to the 32 or 64bit kernel
	mcopy -n -o -i $@ out/$(ARCH)/kinit.img ::/KINIT.IMG
	mcopy -n -o -i $@ out/$(ARCH)/kernel.img ::/KERNEL.IMG

usb: $(BOOT_IMG)
	sudo dd if=$^ of=$(USB_PATH) bs=1M status=progress

xxd: $(DUMP_FILES)

clean:
	rm -rf out/*
	rm -rf *.dump
	rm -rf *.mem
	rm -rf dump
