ARCH = x86
BOOT_BIN = out/$(ARCH)/boot.bin
BOOT2_BIN = out/$(ARCH)/boot2.bin
BOOT_BIN_NAME = NOVALDR.BIN
BOOT_IMG = out/$(ARCH)/boot.img
QEMU_ARGS = -drive format=raw,file=$(BOOT_IMG)
BOOT_INCLUDES = $(wildcard src/arch/$(ARCH)/16bit/*.asm)
MEM_FILES = $(wildcard *.mem)
DUMP_FILES = $(patsubst %.mem, %.dump, $(MEM_FILES))

BOOT_BINARIES = $(BOOT_BIN) $(BOOT2_BIN)

USB_PATH = /dev/sdb
USB_MOUNT = /media/dmtrllv/8683-85AE

DUMP_BYTES = 512
DUMP_FILE = $(USB_PATH)

out/x86/%.bin: src/arch/x86/%.asm
	mkdir -p $(@D)
	nasm -f bin $^ -o $@ -i src/arch/$(ARCH)

%.dump: %.mem
	xxd $^ > $@

run:
	rm -rf out/*
	make boot

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

$(BOOT_IMG): $(BOOT_BINARIES)
	test -e $@ || mkfs.fat -F 16 -C $@ 256000
	# copy the first 3 bytes
	dd bs=1 if=$(BOOT_BIN) count=3 of=$@ conv=notrunc
	# skip 91 bytes from out boot.bin because this is the BPB created by the formatter
	dd bs=1 skip=62 if=$(BOOT_BIN) iflag=skip_bytes of=$@ seek=62 conv=notrunc
	mcopy -n -o -i $@ $(BOOT2_BIN) ::/$(BOOT_BIN_NAME)
	mcopy -n -o -i $@ long-text-file.txt ::/TEST.TXT

dump: $(DUMP_FILE)
	sudo xxd -l $(DUMP_BYTES) $^ > $@

usb: $(BOOT_IMG)
	sudo dd if=$^ of=$(USB_PATH) bs=1M status=progress

xxd: $(DUMP_FILES)

clean:
	rm -rf out/*
	rm -rf *.dump
	rm -rf *.mem
	rm -rf dump
