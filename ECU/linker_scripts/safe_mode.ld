MEMORY
{
  FLASH_SAFE (rx) : ORIGIN = 0x080E0000, LENGTH = 128K
  RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 256K
}

SECTIONS
{
  .isr_vector : { KEEP(*(.isr_vector)) } > FLASH_SAFE
  .text : { *(.text*) *(.rodata*) } > FLASH_SAFE
  .data : { *(.data*) } > RAM AT > FLASH_SAFE
  .bss  : { *(.bss*)  *(COMMON) } > RAM
}
