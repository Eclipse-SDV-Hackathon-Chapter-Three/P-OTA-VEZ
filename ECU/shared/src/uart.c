#include "uart.h"

UART_HandleTypeDef huart1;  // Handle global para UART1

static void UART_MspInit(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Ativa clocks
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();  // Ajusta para o teu GPIO (ex.: GPIOA para PA9/PA10)

    // Configura pins TX/RX
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;  // TX=PA9, RX=PA10 (confirma no datasheet)
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;  // AF7 para USART1 em STM32F412
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void UART_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK) {
        // Erro: adiciona handling (ex.: loop infinito ou LED)
        while(1);
    }

    UART_MspInit();  // Configura GPIO e clocks
}
