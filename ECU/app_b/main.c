#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdio.h>

// Mesma inicialização UART que no bootloader (copia a função UART_Init e _write)

int main(void) {
    HAL_Init();
    UART_Init();
    printf("Boot na partição App B\n");
    while (1) {
        HAL_Delay(1000);
    }
}