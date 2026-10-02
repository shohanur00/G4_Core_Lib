#ifndef UART_HAL_H
#define UART_HAL_H

#include <stdint.h>
#include "stm32g431xx.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * UART Initialization
 * ========================================================================== */

void UART_HAL_Init(
    USART_TypeDef *uart,
    uint8_t        tx_enable,
    uint8_t        rx_enable,
    uint32_t       baudrate,
    uint8_t        parity,
    uint8_t        stop_bits
);


/* ============================================================================
 * UART Status
 * ========================================================================== */

uint8_t UART_HAL_Rx_Ready(
    USART_TypeDef *uart
);

uint8_t UART_HAL_Tx_Ready(
    USART_TypeDef *uart
);

uint8_t UART_HAL_Tx_Complete(
    USART_TypeDef *uart
);


/* ============================================================================
 * UART Data
 * ========================================================================== */

uint8_t UART_HAL_ReadByte(
    USART_TypeDef *uart
);

void UART_HAL_WriteChar(
    USART_TypeDef *uart,
    char           ch
);

void UART_HAL_Write(
    USART_TypeDef *uart,
    const uint8_t *data,
    uint32_t       length
);


/* ============================================================================
 * UART Interrupt
 * ========================================================================== */

void UART_HAL_Rx_Interrupt_Enable(
    USART_TypeDef *uart
);

void UART_HAL_Rx_Interrupt_Disable(
    USART_TypeDef *uart
);

void UART_HAL_Tx_Interrupt_Enable(
    USART_TypeDef *uart
);

void UART_HAL_Tx_Interrupt_Disable(
    USART_TypeDef *uart
);


/* ============================================================================
 * UART DMA
 * ========================================================================== */

void UART_HAL_DMA_Tx_Init(
    USART_TypeDef *uart,
    uint8_t        dma_channel,
    uint32_t       dma_request
);

void UART_HAL_DMA_Rx_Init(
    USART_TypeDef *uart,
    uint8_t        dma_channel,
    uint32_t       dma_request
);

void UART_HAL_DMA_Write(
    USART_TypeDef *uart,
    const uint8_t *data,
    uint32_t       length
);



bool UART_HAL_Rx_IRQHandler(
    USART_TypeDef *uart,
    uint8_t       *data
);


#ifdef __cplusplus
}
#endif

#endif /* UART_HAL_H */