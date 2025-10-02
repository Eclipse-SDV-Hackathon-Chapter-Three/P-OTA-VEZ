#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdio.h>
#include <stdint.h>

// Endereços das partições (ajusta conforme .ld)
#define APP_A_BASE 0x08020000  // Exemplo

UART_HandleTypeDef huart1;

int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

void UART_Init(void) {
    __HAL_RCC_USART1_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_Init = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_Init.Pin = GPIO_PIN_9 | GPIO_PIN_10;  // TX/RX
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_Init);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&huart1);
}

void jump_to_app(uint32_t app_address) {
    uint32_t *reset_vector = (uint32_t *)(app_address + 4);  // Reset handler
    void (*app_reset_handler)(void) = (void *)(*reset_vector);
    __set_MSP(*(uint32_t *)app_address);  // Stack pointer
    app_reset_handler();  // Salta
}

int main(void) {
    HAL_Init();
    // SystemClock_Config();  // Copia do teu system_stm32f4xx.c

    UART_Init();
    printf("Bootloader iniciado\r\n");

    // Verifica e salta (exemplo simples)
    printf("Saltando para App A...\r\n");
    jump_to_app(APP_A_BASE);

    while (1) {  // Se falhar
        HAL_Delay(1000);
        printf("Bootloader rodando...\r\n");
    }
    return 0;
}
