.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global g_pfnVectors
.global Reset_Handler

/* Linker symbols */
.word _estack

/* Vector Table */
.section .isr_vector,"a",%progbits
g_pfnVectors:
  .word _estack              /* Initial stack pointer */
  .word Reset_Handler        /* Reset handler */
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

/* Default handlers */
.section .text.Default_Handler,"ax",%progbits
Default_Handler:
  b .

/* Weak aliases for exception handlers */
.weak NMI_Handler
.weak HardFault_Handler
.weak MemManage_Handler
.weak BusFault_Handler
.weak UsageFault_Handler
.weak SVC_Handler
.weak DebugMon_Handler
.weak PendSV_Handler
.weak SysTick_Handler

NMI_Handler:
HardFault_Handler:
MemManage_Handler:
BusFault_Handler:
UsageFault_Handler:
SVC_Handler:
DebugMon_Handler:
PendSV_Handler:
SysTick_Handler:
  b Default_Handler

/* Reset Handler */
.section .text.Reset_Handler,"ax",%progbits
Reset_Handler:
  /* Init data and bss */
  ldr r0, =_sidata
  ldr r1, =_sdata
  ldr r2, =_edata
1:
  cmp r1, r2
  ittt lt
  ldrlt r3, [r0], #4
  strlt r3, [r1], #4
  blt 1b

  ldr r0, =_sbss
  ldr r1, =_ebss
  movs r2, #0
2:
  cmp r0, r1
  itt lt
  strlt r2, [r0], #4
  blt 2b

  bl main

LoopForever:
  b LoopForever
