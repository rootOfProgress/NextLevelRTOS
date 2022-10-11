
/* generall memory info; manual cortex m4 page 30 */
MEMORY
{
  /* see page 59 at reference manual */
  /* flash area starts @0800000 */
  FLASH (rx) : ORIGIN = 0x20000fff, LENGTH = 512K

  SRAM (rwx) : ORIGIN = 0x200000FF, LENGTH = 39745
}

/* The Entry section expects the symbol name of the first executable 
piece of code which will be loaded into the processor. Logically it is
the first part of the .text section. */
ENTRY(main);

SECTIONS
{
  .text :
  {
    KEEP(*(.main))
    *(.text .text.*);
  } > FLASH
  /* static variables or string literals need .bss .data and .rodata sections */

  /* .rodata is read only data; it is where global constants are placed. */
  .rodata :
  {
    *(.rodata .rodata.*);
  } > FLASH

  /* .bss is where uninitialized global variables are placed. */
  .bss :
  {
    _sbss = .;
    *(.bss .bss.*);
    _ebss = .;
  } > SRAM

  /* .data is where global variables that are initialized at compile time are placed. */
  .data : AT(ADDR(.rodata) + SIZEOF(.rodata))
  {
    _sdata = .;
    *(.data .data.*);
    _edata = .;
  } > SRAM

  _sidata = LOADADDR(.data);

  /DISCARD/ :
  {
    *(.ARM.exidx .ARM.exidx.*);
  }
}
