ENTRY(kinit)
OUTPUT_FORMAT("binary")
SECTIONS
{
	. = 0x2000;
 
	.text :
	{
		*(.text.kinit);
		*(.text.asm);
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
