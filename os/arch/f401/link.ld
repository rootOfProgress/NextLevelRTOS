ram_size = 64K;
ram_start = 0x20000000;
EXTERN(bootstrap)
ENTRY(bootstrap)

MEMORY
{
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
	RAM (rwx) : ORIGIN = ram_start, LENGTH = ram_size
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
	
	.data :
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

	stack_top = ORIGIN(RAM) + LENGTH(RAM);
    _sidata = LOADADDR(.data);

}
