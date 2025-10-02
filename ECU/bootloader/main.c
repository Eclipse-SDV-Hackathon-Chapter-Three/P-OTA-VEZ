#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdio.h>
#include <stdint.h>

// Endereço base da App A (confirme no linker script)
#define APP_A_BASE       0x08020000

UART_HandleTypeDef huart1;

// Função para redirecionar printf para UART
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

typedef void (*pFunction)(void);

void jump_to_app(uint32_t app_address) {
    uint32_t *vector_table = (uint32_t *)app_address;
    uint32_t jump_address = vector_table[1]; // Reset_Handler address
    pFunction app_reset_handler = (pFunction)jump_address;

    // Desabilita todas as interrupções antes do salto (bom para segurança)
    __disable_irq();

    // Set stack pointer to start of app
    __set_MSP(vector_table[0]);

    // Set vector table offset register to app base
    SCB->VTOR = app_address;

    // Jump para Reset_Handler da app
    app_reset_handler();
}

int main(void) {
    HAL_Init();
    // SystemClock_Config();  // Se já tiver, use para configurar o clock do microcontrolador

    UART_Init();
    printf("Bootloader iniciado\r\n");

    printf("Saltando para App A...\r\n");
    jump_to_app(APP_A_BASE);

    while (1) {
        HAL_Delay(1000);
        printf("Bootloader rodando...\r\n");  // Só executa se o jump falhar
    }
    return 0;
}
