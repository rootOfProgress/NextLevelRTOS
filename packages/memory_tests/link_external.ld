STARTADRESS = 0x20001583;
SIZE = 3813;

MEMORY
{
  RAM (rx) : ORIGIN = STARTADRESS, LENGTH = SIZE
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

  .data :
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
