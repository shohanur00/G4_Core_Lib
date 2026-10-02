#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32g431xx.h"
#include <stdbool.h>

/* ============================================================================
 * UART Enable
 * ========================================================================== */

typedef enum
{
    UART_DISABLE = 0U,
    UART_ENABLE  = 1U

} UART_Enable_t;


/* ============================================================================
 * UART Parity
 * ========================================================================== */

typedef enum
{
    UART_PARITY_NONE = 0U,
    UART_PARITY_EVEN,
    UART_PARITY_ODD

} UART_Parity_t;


/* ============================================================================
 * UART Stop Bits
 * ========================================================================== */

typedef enum
{
    UART_STOP_BITS_1 = 0U,
    UART_STOP_BITS_2

} UART_StopBits_t;


/* ============================================================================
 * UART Initialization
 * ========================================================================== */

/**
 * @brief Initialize UART peripheral.
 *
 * @param instance   UART instance number.
 * @param tx_enable  Enable/disable transmitter.
 * @param rx_enable  Enable/disable receiver.
 * @param baudrate   UART baud rate.
 * @param parity     UART parity configuration.
 * @param stop_bits  UART stop-bit configuration.
 */
void UART_Driver_Init(
    uint8_t         instance,
    UART_Enable_t   tx_enable,
    UART_Enable_t   rx_enable,
    uint32_t        baudrate,
    UART_Parity_t   parity,
    UART_StopBits_t stop_bits
);


/* ============================================================================
 * UART RX Interrupt
 * ========================================================================== */

/**
 * @brief Enable UART RX interrupt.
 *
 * @param instance UART instance number.
 */
void UART_Driver_Rx_Interrupt_Enable(
    uint8_t instance
);


/**
 * @brief Disable UART RX interrupt.
 *
 * @param instance UART instance number.
 */
void UART_Driver_Rx_Interrupt_Disable(
    uint8_t instance
);


/* ============================================================================
 * UART TX Interrupt
 * ========================================================================== */

/**
 * @brief Enable UART TX interrupt.
 *
 * @param instance UART instance number.
 */
void UART_Driver_Tx_Interrupt_Enable(
    uint8_t instance
);


/**
 * @brief Disable UART TX interrupt.
 *
 * @param instance UART instance number.
 */
void UART_Driver_Tx_Interrupt_Disable(
    uint8_t instance
);


/* ============================================================================
 * UART RX
 * ========================================================================== */

/**
 * @brief Check whether UART RX data is ready.
 *
 * @param instance UART instance number.
 *
 * @return Non-zero if data is available, otherwise 0.
 */
uint8_t UART_Driver_Rx_Ready(
    uint8_t instance
);


/**
 * @brief Read one byte from UART.
 *
 * @param instance UART instance number.
 *
 * @return Received byte.
 */
bool UART_Driver_ReadByte(
    uint8_t instance,
    uint8_t *data
);


/* ============================================================================
 * UART TX
 * ========================================================================== */

/**
 * @brief Check whether UART TX is ready to accept data.
 *
 * @param instance UART instance number.
 *
 * @return Non-zero if TX is ready, otherwise 0.
 */
uint8_t UART_Driver_Tx_Ready(
    uint8_t instance
);


/**
 * @brief Check whether UART transmission is complete.
 *
 * @param instance UART instance number.
 *
 * @return Non-zero if transmission is complete, otherwise 0.
 */
uint8_t UART_Driver_Tx_Complete(
    uint8_t instance
);


/**
 * @brief Write one character to UART.
 *
 * @param instance UART instance number.
 * @param ch       Character to transmit.
 */
void UART_Driver_WriteChar(
    uint8_t instance,
    char    ch
);


/**
 * @brief Write a data buffer through UART.
 *
 * @param instance UART instance number.
 * @param data     Pointer to data buffer.
 * @param length   Number of bytes to transmit.
 */
void UART_Driver_Write(
    uint8_t        instance,
    const uint8_t *data,
    uint32_t       length
);


/* ============================================================================
 * UART DMA TX
 * ========================================================================== */

/**
 * @brief Initialize UART DMA TX.
 *
 * @param instance    UART instance number.
 * @param dma_channel DMA channel number.
 * @param dma_request DMA request selection.
 */
void UART_Driver_DMA_Tx_Init(
    uint8_t  instance,
    uint8_t  dma_channel,
    uint32_t dma_request
);


/* ============================================================================
 * UART DMA RX
 * ========================================================================== */

/**
 * @brief Initialize UART DMA RX.
 *
 * @param instance    UART instance number.
 * @param dma_channel DMA channel number.
 * @param dma_request DMA request selection.
 */
void UART_Driver_DMA_Rx_Init(
    uint8_t  instance,
    uint8_t  dma_channel,
    uint32_t dma_request
);


/* ============================================================================
 * UART DMA TX
 * ========================================================================== */

/**
 * @brief Transmit data using UART DMA.
 *
 * @param instance UART instance number.
 * @param data     Pointer to data buffer.
 * @param length   Number of bytes to transmit.
 */
void UART_Driver_DMA_Write(
    uint8_t        instance,
    const uint8_t *data,
    uint32_t       length
);


/* ============================================================================
 * UART IRQ Handler
 * ========================================================================== */

/**
 * @brief Handle UART interrupt.
 *
 * @param uart Pointer to UART peripheral instance.
 */
void UART_Driver_IRQHandler(
    USART_TypeDef *uart
);


#ifdef __cplusplus
}
#endif

#endif /* UART_DRIVER_H */