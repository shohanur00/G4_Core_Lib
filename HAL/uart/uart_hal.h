#ifndef UART_HAL_H
#define UART_HAL_H

#include <stdint.h>
#include "stm32g431xx.h"


/* ============================================================================
 * UART HAL Initialization
 * ========================================================================== */

/**
 * @brief Initialize UART/USART peripheral.
 *
 * @param uart       UART/USART peripheral instance.
 * @param tx_enable  Enable/disable transmitter.
 * @param rx_enable  Enable/disable receiver.
 * @param baudrate   Communication baud rate.
 * @param parity     Parity configuration.
 * @param stop_bits  Stop-bit configuration.
 *
 * @note GPIO configuration is handled by the GPIO Driver.
 */
void UART_HAL_Init(
    USART_TypeDef *uart,
    uint8_t   tx_enable,
    uint8_t   rx_enable,
    uint32_t  baudrate,
    uint8_t   parity,
    uint8_t   stop_bits
);


/* ============================================================================
 * UART RX Interrupt Enable
 * ========================================================================== */

void UART_HAL_Rx_Interrupt_Enable(
    USART_TypeDef *uart
);


/* ============================================================================
 * UART TX Interrupt Enable
 * ========================================================================== */

void UART_HAL_Tx_Interrupt_Enable(
    USART_TypeDef *uart
);

/* ============================================================================
 * UART Write
 * ========================================================================== */

/**
 * @brief Transmit data through UART.
 *
 * @param uart    UART/USART peripheral instance.
 * @param data    Data buffer.
 * @param length  Number of bytes to transmit.
 */
void UART_HAL_Write(
    USART_TypeDef *uart,
    const uint8_t *data,
    uint32_t       length
);


/* ============================================================================
 * UART DMA TX Initialization
 * ========================================================================== */

/**
 * @brief Initialize UART DMA transmission.
 *
 * @param uart         UART/USART peripheral instance.
 * @param dma_channel  DMA channel number.
 * @param dma_request  DMAMUX request selection.
 */
void UART_HAL_DMA_Tx_Init(
    USART_TypeDef *uart,
    uint8_t        dma_channel,
    uint32_t       dma_request
);


/* ============================================================================
 * UART DMA RX Initialization
 * ========================================================================== */

/**
 * @brief Initialize UART DMA reception.
 *
 * @param uart         UART/USART peripheral instance.
 * @param dma_channel  DMA channel number.
 * @param dma_request  DMAMUX request selection.
 */
void UART_HAL_DMA_Rx_Init(
    USART_TypeDef *uart,
    uint8_t        dma_channel,
    uint32_t       dma_request
);

#endif /* UART_HAL_H */