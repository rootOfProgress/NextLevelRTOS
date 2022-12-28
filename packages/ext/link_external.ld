
/* generall memory info; manual cortex m4 page 30 */
MEMORY
{
  /* see page 59 at reference manual */
  /* 0x20002d9b flash area starts @0800000 */
  /*RAM (rx) : ORIGIN = 0x2000232b, LENGTH = 512*/
  RAM (rx) : ORIGIN = 0x20004007, LENGTH = 1024
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
  } > RAM
  /* static variables or string literals need .bss .data and .rodata sections */

  /* .rodata is read only data; it is where global constants are placed. */
  .rodata :
  {
    *(.rodata .rodata.*);
  } > RAM

  /* .bss is where uninitialized global variables are placed. */
  .bss :
  {
    _sbss = .;
    *(.bss .bss.*);
    _ebss = .;
  } > RAM

  /* .data is where global variables that are initialized at compile time are placed. */
  .data : AT(ADDR(.rodata) + SIZEOF(.rodata))
  {
    _sdata = .;
    *(.data .data.*);
    _edata = .;
  } > RAM

  /DISCARD/ :
  {
    *(.ARM.exidx .ARM.exidx.*);
  }
}
