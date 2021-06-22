ENTRY(kmain)
OUTPUT_FORMAT("binary")
SECTIONS
{
	. = 0x3000;
 
	.text : ALIGN(4K)
	{
		*(.text.kmain);
		*(.text);
	}
 
	.rodata : ALIGN(4K)
	{
		*(.rodata);
	}
 
	.data : ALIGN(4K)
	{
		*(.data);
	}
 
	.bss : ALIGN(4K)
	{
		*(COMMON);
		*(.bss);
	}
}
