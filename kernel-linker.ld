OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
SECTIONS
{
	. = 0x30000;
 
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text.kmain)
		code = .;
		*(.text);
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		rodata = .;
		*(.rodata);
	}
 
	.data BLOCK(4K) : ALIGN(4K)
	{
		data = .;
		*(.data);
	}
 
	.bss BLOCK(4K) : ALIGN(4K)
	{
		bss = .;
		*(COMMON);
		*(.bss);
	}

	/DISCARD/ : 
	{
		*(.eh_frame);
		*(.comment);
		*(.note*)
        *(.indent)
        *(.comment)
        *(.stab)
        *(.stabstr)
	}

    end = .;
}