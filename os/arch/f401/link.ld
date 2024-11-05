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
		/* *(.rodata)
		*(.rodata.*) */
		/* _sromdev = .;
		_eromdev = .;
		_sidata = .; */
	} >FLASH

	/* Constant data into ROM memory*/
	.rodata :
	{
		. = ALIGN(4);
		*(.rodata)         /* .rodata sections (constants, strings, etc.) */
		*(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
		. = ALIGN(4);
	} >FLASH
	
	/* Used by the startup to initialize data */
	_sidata = LOADADDR(.data);

	/* Initialized data sections into RAM memory */
	.data : 
	{
		. = ALIGN(4);
		_sdata = .;        /* create a global symbol at data start */
		*(.data)           /* .data sections */
		*(.data*)          /* .data* sections */

		. = ALIGN(4);
		_edata = .;        /* define a global symbol at data end */
	} >RAM AT> FLASH
	
	/* .data :
	{
		_sdata = .;
		*(.data)
		*(.data*)
		_edata = .;
	} >RAM */

	.bss :
	{
		_sbss = .;
		*(.bss)
		_ebss = .;
	} >RAM

	stack_top = ORIGIN(RAM) + LENGTH(RAM);
    /* _sidata = LOADADDR(.data); */

}
