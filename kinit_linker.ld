ENTRY(kinit)
OUTPUT_FORMAT("binary")
SECTIONS
{
	. = 0x2000;
 
	.text :
	{
		*(.text.kinit);
		code = .; _code = .; __code = .;
		*(.text);
	}
 
	.rodata :
	{
		rodata = .; _rodata = .; __rodata = .;
		*(.rodata);
	}
 
	.data :
	{
		data = .; _data = .; __data = .;
		*(.data);
	}
 
	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(COMMON);
		*(.bss);
	}

    end = .; _end = .; __end = .;
}
