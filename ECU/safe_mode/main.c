#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdio.h>

// Mesma inicialização UART que no bootloader
int main(void) {
    HAL_Init();
    UART_Init();
    printf("Boot no Safe Mode\n");
    while (1) {
        HAL_Delay(1000);
    }
}

