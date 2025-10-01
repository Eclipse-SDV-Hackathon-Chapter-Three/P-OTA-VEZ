MEMORY
{
  FLASH_APP_A (rx) : ORIGIN = 0x08020000, LENGTH = 384K
  RAM (rwx)        : ORIGIN = 0x20000000, LENGTH = 256K
}

SECTIONS
{
  .isr_vector : { KEEP(*(.isr_vector)) } > FLASH_APP_A
  .text : { *(.text*) *(.rodata*) } > FLASH_APP_A
  .data : { *(.data*) } > RAM AT > FLASH_APP_A
  .bss  : { *(.bss*)  *(COMMON) } > RAM
}
