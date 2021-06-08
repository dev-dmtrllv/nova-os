ARCH = x86

BOOT_BIN = out/$(ARCH)/boot.bin

QEMU_ARGS = -drive format=raw,file=$(BOOT_BIN)

$(BOOT_BIN): src/arch/$(ARCH)/boot.asm
	mkdir -p $(@D)
	nasm -f bin $^ -o $@

all: $(BOOT_BIN)
	echo $^

run: $(BOOT_BIN)
ifeq ($(ARCH), x86)
	qemu-system-i386 $(QEMU_ARGS)
endif
ifeq ($(ARCH), x86_64)
	qemu-system-x86_64 $(QEMU_ARGS)
endif
ifeq ($(ARCH), arm)
	qemu-system-arm $(QEMU_ARGS)
endif
