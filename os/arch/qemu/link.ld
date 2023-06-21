ram_size = 40000;
ram_start = 0x20000008;
metadata = 0x20000000;
EXTERN(bootstrap)
ENTRY(bootstrap)

MEMORY
{
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 32K
	RAM (rwx) : ORIGIN = ram_start, LENGTH = ram_size
	META (rwx) : ORIGIN = metadata, LENGTH = 8
}

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
		_sromdev = .;
		_eromdev = .;
		_sidata = .;
	} >FLASH

	.meta :
	{
		KEEP(*(.meta))
	} >META

	.data : AT(_sidata)
	{
		_sdata = .;
		*(.data)
		*(.data*)
		_edata = .;
	} >RAM

	.bss :
	{
		_sbss = .;
		*(.bss)
		_ebss = .;
	} >RAM

	/DISCARD/ :
	{
	    *(.ARM.exidx*)
	    *(.gnu.linkonce.armexidx.*)
	}

	stack_top = ORIGIN(RAM) + LENGTH(RAM);
	_sidata = LOADADDR(.data);
}
