# Nova OS


## Project Structure
----
```
+---+ docs
|
+---+ include
|	|
|	+---+ arch
|	|
|	+---+ kernel
|	|
|	+---+ driver
|	|
|	+---+ lib
|
+---+ src
	|
	+---+ arch (specific cpu code, bootloader)
	|	|
	|	+---+ x86
	|	|
	|	+---+ x64
	|	|
	|	+---+ arm
	|
	+---+ kernel
	|	|
	|	+---+ core
	|	|
	|	+---+ memory management (mm)
	|	|
	|	+---+ exec interface (ELF/DLL)
	|	|
	|	+---+ access control and permission (UAC)
	|	|
	|	+---+ networking (TCP/IP)
	|	|
	|	+---+ resource management
	|	|
	|	+---+ scheduler (processes, threads, multitasking)
	|	|
	|	+---+ std interfaces (stdin, stdout, stderr)
	|	|
	|	+---+ system calls
	|	|
	|	+---+ kernel drivers (native interface & EDI)
	|	|
	|	+---+ IPC (pipes, sockets, signals, semaphores, mutexes, shared memory)
	|
	+---+ lib (malloc, free, printf etc.)
	|
	+---+ drivers
		|
		+---+ HDD (ATA, SATA, SCSI)
		|
		+---+ networking (8023 driver)
		|
		+---+ usb (EHCI, OHCI, UHCI)
		|
		+---+ mouse/trackpad
		|
		+---+ keyboard
		|
		+---+ filesystems (vfs, fat16, fat32, ext2, ntfs)
		|
		+---+ screen (VGA, VESA)
	
```

## Step 1 (writing the first stage bootloader)
When the computer starts the bios will look for an bootable drive. It checks the last 4 bytes of the first sector of every drive. If those bytes matches 0xAA55 it will load the first sector from the drive into memory (at location 0x7c00) and starts executing it. The goal of the first stage bootloader is to load the 2nd stage since the first stage is only 512 bytes in size (too small to make a good working bootloader). The first stage will search for the 2nd bootloader binary on the drive and will load it into memory. Then we pass control to the 2nd bootloader.



# Bootloader Memory Layout
```
+-- 0x30000 	Kernel
|
|
+-- 0x10000 	Kernel Initialization
|
|
+-- 0x8000 		Fat-16 Directory Entry Table and File Allocation Table (FAT)
|
+-- 0x7e00		list of file clusters to load
+-- 0x7e00		|End first boot stage
|				|
|				|
+-- 0x7C00 		|First boot stage
|
|
+-- 0x3000 		Bios Kernel Info Area (for the kinit and kernel)
|
|
+-- 0x1000 		Second boot stage
|
|
+-- 0x800		Global descriptor Table
|
|
+-- 0x0			Interrupt Vector Table
```

## Bios Kernel Info Area
This area contains data and pointers obtained from the bios to provide to the kinit and kernel. It makes it way easier to just ask the bios (in real mode) for information than to write those routines in protected mode.