STARTADRESS = 0x2000150f;
SIZE = 8000;

MEMORY
{
  RAM (rx) : ORIGIN = STARTADRESS, LENGTH = SIZE
  ISR1 (rx) : ORIGIN = 0x20005000, LENGTH = 512
  ISR2 (rx) : ORIGIN = 0x20006000, LENGTH = 512
}

ENTRY(main);

SECTIONS
{
  .text :
  {
    KEEP(*(.main))
    *(.text .text.*);
  } > RAM

  .isr1 :
  {
    KEEP(*(.isr1))
  } > ISR1

  .isr2 :
  {
    KEEP(*(.isr2))
  } > ISR2

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
