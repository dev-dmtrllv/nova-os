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
		|
		+---+ screen (VGA, VESA)
	
```

## Step 1 (writing the first stage bootloader)
When the computer starts the bios will look for an bootable drive. It checks the last 4 bytes of the first sector of every drive. If those bytes matches 0x55AA it will load the first sector from the drive into memory (at location 0x7c00) and starts executing it. The goal of the first stage bootloader is to load the 2nd stage since the first stage is only 512 bytes in size (too small to make a good working bootloader). The first stage will search for the 2nd bootloader binary on the drive and will load it into memory. Then we pass control to the 2nd bootloader.
