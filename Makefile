ARCH = x86
BOOT_BIN = out/$(ARCH)/boot.bin
BOOT_IMG = out/$(ARCH)/boot.img
QEMU_ARGS = -drive format=raw,file=$(BOOT_IMG)


$(BOOT_BIN): src/arch/$(ARCH)/boot.asm
	mkdir -p $(@D)
	nasm -f bin $^ -o $@ -i src/arch/$(ARCH)

run: $(BOOT_IMG)
ifeq ($(ARCH), x86)
	qemu-system-i386 $(QEMU_ARGS)
endif
ifeq ($(ARCH), x86_64)
	qemu-system-x86_64 $(QEMU_ARGS)
endif
ifeq ($(ARCH), arm)
	qemu-system-arm $(QEMU_ARGS)
endif

$(BOOT_IMG): $(BOOT_BIN)
	test -e $@ || mkfs.fat -F 16 -C $@ 512000
	# copy the first 3 bytes
	dd bs=1 if=$(BOOT_BIN) count=3 of=$@ conv=notrunc
	# skip 91 bytes from out boot.bin because this is the BPB created by the formatter
	dd bs=1 skip=62 if=$(BOOT_BIN) iflag=skip_bytes of=$@ seek=62 conv=notrunc

clean:
	rm -rf out/*
	rm -rf *.dump
	rm -rf *.mem
