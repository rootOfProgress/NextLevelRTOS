STARTADRESS = 0x20002b45;

MEMORY
{
  RAM (rx) : ORIGIN = STARTADRESS, LENGTH = 1024
}

ENTRY(main);

SECTIONS
{
  .text :
  {
    KEEP(*(.main))
    *(.text .text.*);
  } > RAM

  .rodata :
  {
    *(.rodata .rodata.*);
  } > RAM

  .bss :
  {
    _sbss = .;
    *(.bss .bss.*);
    _ebss = .;
  } > RAM

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
