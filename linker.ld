ENTRY(kmain)
OUTPUT_FORMAT("binary")
SECTIONS
{
	. = 0x10000;
 
	.text :
	{
		*(.text.kmain);
		*(.text);
	}
 
	.rodata :
	{
		*(.rodata);
	}
 
	.data :
	{
		*(.data);
	}
 
	.bss :
	{
		*(COMMON);
		*(.bss);
	}
}
