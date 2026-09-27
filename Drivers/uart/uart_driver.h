#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>

/* ============================================================================
 * UART Enable / Disable
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
    UART_PARITY_EVEN = 1U,
    UART_PARITY_ODD  = 2U
} UART_Parity_t;


/* ============================================================================
 * UART Stop Bits
 * ========================================================================== */

typedef enum
{
    UART_STOPBITS_1 = 0U,
    UART_STOPBITS_2 = 1U
} UART_StopBits_t;


/* ============================================================================
 * UART Initialization
 * ========================================================================== */

void UART_Driver_Init(
    uint8_t       instance,
    UART_Enable_t tx_enable,
    UART_Enable_t rx_enable,
    uint32_t      baudrate,
    UART_Parity_t parity,
    UART_StopBits_t stop_bits
);



/* ============================================================================
 * UART RX Interrupt Enable
 * ========================================================================== */

void UART_Driver_Rx_Interrupt_Enable(
    uint8_t instance
);


/* ============================================================================
 * UART TX Interrupt Enable
 * ========================================================================== */

void UART_Driver_Tx_Interrupt_Enable(
    uint8_t instance
);

/* ============================================================================
 * UART Write
 * ========================================================================== */

void UART_Driver_Write(
    uint8_t        instance,
    const uint8_t *data,
    uint32_t       length
);


/* ============================================================================
 * UART DMA TX Initialization
 * ========================================================================== */

void UART_Driver_DMA_Tx_Init(
    uint8_t  instance,
    uint8_t  dma_channel,
    uint32_t dma_request
);


/* ============================================================================
 * UART DMA RX Initialization
 * ========================================================================== */

void UART_Driver_DMA_Rx_Init(
    uint8_t  instance,
    uint8_t  dma_channel,
    uint32_t dma_request
);

#endif /* UART_DRIVER_H */