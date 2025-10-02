#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdio.h>

// Mesma inicialização UART que no bootloader

int main(void) {
    HAL_Init();
    // SystemClock_Config();

    UART_Init();
    printf("Safe Mode iniciado\r\n");

    while (1) {
        HAL_Delay(1000);
        printf("Safe Mode rodando... (recuperação)\r\n");
    }
    return 0;
}
