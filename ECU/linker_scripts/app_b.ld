MEMORY
{
  FLASH_APP_B (rx) : ORIGIN = 0x08080000, LENGTH = 384K
  RAM (rwx)        : ORIGIN = 0x20000000, LENGTH = 256K
}

SECTIONS
{
  .isr_vector : { KEEP(*(.isr_vector)) } > FLASH_APP_B
  .text : { *(.text*) *(.rodata*) } > FLASH_APP_B
  .data : { *(.data*) } > RAM AT > FLASH_APP_B
  .bss  : { *(.bss*)  *(COMMON) } > RAM
}
