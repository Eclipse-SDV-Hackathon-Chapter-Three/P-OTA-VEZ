MEMORY
{
  FLASH_BOOT (rx) : ORIGIN = 0x08000000, LENGTH = 64K
  RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 256K
}

SECTIONS
{
  .isr_vector : { KEEP(*(.isr_vector)) } > FLASH_BOOT
  .text : { *(.text*) *(.rodata*) } > FLASH_BOOT
  .data : { *(.data*) } > RAM AT > FLASH_BOOT
  .bss  : { *(.bss*)  *(COMMON) } > RAM
}
