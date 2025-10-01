#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdio.h>

// Mesma inicialização UART que no bootloader (copia a função UART_Init e _write)

int main(void) {
    HAL_Init();
    // SystemClock_Config();

    UART_Init();
    printf("App A iniciado\r\n");

    // Adiciona o teu código real aqui (ex.: cloudconfig, sntpclient do teu app original)

    while (1) {
        HAL_Delay(1000);
        printf("App A rodando...\r\n");
    }
    return 0;
}
