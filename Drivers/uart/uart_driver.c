#include "uart_driver.h"
#include "uart/uart_hal.h"

/* ============================================================================
 * UART Initialization
 * ========================================================================== */

void UART_Driver_Init(
    uint8_t         instance,
    UART_Enable_t   tx_enable,
    UART_Enable_t   rx_enable,
    uint32_t        baudrate,
    UART_Parity_t   parity,
    UART_StopBits_t stop_bits
)
{
    switch (instance)
    {
        case 1U:

            UART_HAL_Init(
                USART1,
                tx_enable,
                rx_enable,
                baudrate,
                parity,
                stop_bits
            );

            break;


        case 2U:

            UART_HAL_Init(
                USART2,
                tx_enable,
                rx_enable,
                baudrate,
                parity,
                stop_bits
            );

            break;


        case 3U:

            UART_HAL_Init(
                USART3,
                tx_enable,
                rx_enable,
                baudrate,
                parity,
                stop_bits
            );

            break;


        case 4U:

            UART_HAL_Init(
                UART4,
                tx_enable,
                rx_enable,
                baudrate,
                parity,
                stop_bits
            );

            break;


        case 5U:

            UART_HAL_Init(
                UART5,
                tx_enable,
                rx_enable,
                baudrate,
                parity,
                stop_bits
            );

            break;


        case 6U:

            UART_HAL_Init(
                LPUART1,
                tx_enable,
                rx_enable,
                baudrate,
                parity,
                stop_bits
            );

            break;


        default:
            break;
    }
}


/* ============================================================================
 * UART RX Interrupt Enable
 * ========================================================================== */

void UART_Driver_Rx_Interrupt_Enable(
    uint8_t instance
)
{
    switch (instance)
    {
        case 1U:
            UART_HAL_Rx_Interrupt_Enable(USART1);
            break;

        case 2U:
            UART_HAL_Rx_Interrupt_Enable(USART2);
            break;

        case 3U:
            UART_HAL_Rx_Interrupt_Enable(USART3);
            break;

        case 4U:
            UART_HAL_Rx_Interrupt_Enable(UART4);
            break;

        case 5U:
            UART_HAL_Rx_Interrupt_Enable(UART5);
            break;

        case 6U:
            UART_HAL_Rx_Interrupt_Enable(LPUART1);
            break;

        default:
            break;
    }
}


/* ============================================================================
 * UART TX Interrupt Enable
 * ========================================================================== */

void UART_Driver_Tx_Interrupt_Enable(
    uint8_t instance
)
{
    switch (instance)
    {
        case 1U:
            UART_HAL_Tx_Interrupt_Enable(USART1);
            break;

        case 2U:
            UART_HAL_Tx_Interrupt_Enable(USART2);
            break;

        case 3U:
            UART_HAL_Tx_Interrupt_Enable(USART3);
            break;

        case 4U:
            UART_HAL_Tx_Interrupt_Enable(UART4);
            break;

        case 5U:
            UART_HAL_Tx_Interrupt_Enable(UART5);
            break;

        case 6U:
            UART_HAL_Tx_Interrupt_Enable(LPUART1);
            break;

        default:
            break;
    }
}


/* ============================================================================
 * UART Write
 * ========================================================================== */

void UART_Driver_Write(
    uint8_t        instance,
    const uint8_t *data,
    uint32_t       length
)
{
    switch (instance)
    {
        case 1U:
            UART_HAL_Write(
                USART1,
                data,
                length
            );
            break;

        case 2U:
            UART_HAL_Write(
                USART2,
                data,
                length
            );
            break;

        case 3U:
            UART_HAL_Write(
                USART3,
                data,
                length
            );
            break;

        case 4U:
            UART_HAL_Write(
                UART4,
                data,
                length
            );
            break;

        case 5U:
            UART_HAL_Write(
                UART5,
                data,
                length
            );
            break;

        case 6U:
            UART_HAL_Write(
                LPUART1,
                data,
                length
            );
            break;

        default:
            break;
    }
}


/* ============================================================================
 * UART DMA TX Initialization
 * ========================================================================== */

void UART_Driver_DMA_Tx_Init(
    uint8_t  instance,
    uint8_t  dma_channel,
    uint32_t dma_request
)
{
    switch (instance)
    {
        case 1U:
            UART_HAL_DMA_Tx_Init(
                USART1,
                dma_channel,
                dma_request
            );
            break;

        case 2U:
            UART_HAL_DMA_Tx_Init(
                USART2,
                dma_channel,
                dma_request
            );
            break;

        case 3U:
            UART_HAL_DMA_Tx_Init(
                USART3,
                dma_channel,
                dma_request
            );
            break;

        case 4U:
            UART_HAL_DMA_Tx_Init(
                UART4,
                dma_channel,
                dma_request
            );
            break;

        case 5U:
            UART_HAL_DMA_Tx_Init(
                UART5,
                dma_channel,
                dma_request
            );
            break;

        case 6U:
            UART_HAL_DMA_Tx_Init(
                LPUART1,
                dma_channel,
                dma_request
            );
            break;

        default:
            break;
    }
}


/* ============================================================================
 * UART DMA RX Initialization
 * ========================================================================== */

void UART_Driver_DMA_Rx_Init(
    uint8_t  instance,
    uint8_t  dma_channel,
    uint32_t dma_request
)
{
    switch (instance)
    {
        case 1U:
            UART_HAL_DMA_Rx_Init(
                USART1,
                dma_channel,
                dma_request
            );
            break;

        case 2U:
            UART_HAL_DMA_Rx_Init(
                USART2,
                dma_channel,
                dma_request
            );
            break;

        case 3U:
            UART_HAL_DMA_Rx_Init(
                USART3,
                dma_channel,
                dma_request
            );
            break;

        case 4U:
            UART_HAL_DMA_Rx_Init(
                UART4,
                dma_channel,
                dma_request
            );
            break;

        case 5U:
            UART_HAL_DMA_Rx_Init(
                UART5,
                dma_channel,
                dma_request
            );
            break;

        case 6U:
            UART_HAL_DMA_Rx_Init(
                LPUART1,
                dma_channel,
                dma_request
            );
            break;

        default:
            break;
    }
}