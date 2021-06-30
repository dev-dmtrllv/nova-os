ENTRY(kinit)
OUTPUT_FORMAT("binary")
SECTIONS
{
	. = 0x2000;
 
	.text : ALIGN(4k)
	{
		*(.text.kinit);
		code = .; _code = .; __code = .;
		*(.text);
	}
 
	.rodata : ALIGN(4k)
	{
		rodata = .; _rodata = .; __rodata = .;
		*(.rodata);
	}
 
	.data : ALIGN(4k)
	{
		data = .; _data = .; __data = .;
		*(.data);
	}
 
	.bss : ALIGN(4k)
	{
		bss = .; _bss = .; __bss = .;
		*(COMMON);
		*(.bss);
	}

    end = .; _end = .; __end = .;
}
