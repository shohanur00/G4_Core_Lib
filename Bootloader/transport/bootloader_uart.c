#include "bootloader_uart.h"
#include "uart/uart_driver.h"
#include "board.h"

void Bootloader_UART_Init(void)
{
    /* ========================================================================
     * UART Initialization
     * ====================================================================== */

    UART_Driver_Init(
        BOOTLOADER_UART_INSTANCE,
        BOOTLOADER_UART_TX_ENABLE,
        BOOTLOADER_UART_RX_ENABLE,
        BOOTLOADER_UART_BAUDRATE,
        BOOTLOADER_UART_PARITY,
        BOOTLOADER_UART_STOP_BITS
    );

#if BOOTLOADER_UART_TX_INTERRUPT

    UART_Driver_Tx_Interrupt_Enable(BOOTLOADER_UART_INSTANCE);

#endif


#if BOOTLOADER_UART_RX_INTERRUPT

    UART_Driver_Rx_Interrupt_Enable(BOOTLOADER_UART_INSTANCE);

#endif

    /* ========================================================================
     * DMA TX Initialization
     * ====================================================================== */

#if BOOTLOADER_UART_DMA_TX_ENABLE

    UART_Driver_DMA_Tx_Init(
        BOOTLOADER_UART_INSTANCE,
        BOOTLOADER_UART_DMA_TX_CHANNEL,
        BOOTLOADER_UART_DMA_TX_REQUEST
    );

#endif


    /* ========================================================================
     * DMA RX Initialization
     * ====================================================================== */

#if BOOTLOADER_UART_DMA_RX_ENABLE

    UART_Driver_DMA_Rx_Init(
        BOOTLOADER_UART_INSTANCE,
        BOOTLOADER_UART_DMA_RX_CHANNEL,
        BOOTLOADER_UART_DMA_RX_REQUEST
    );

#endif
}




/* ============================================================================
 * Bootloader UART Write
 * ========================================================================== */

void Bootloader_UART_Write(
    const uint8_t *data,
    uint32_t       length
)
{
#if BOOTLOADER_UART_DMA_TX_ENABLE

    UART_Driver_DMA_Write(
        BOOTLOADER_UART_INSTANCE,
        data,
        length
    );

#else

    UART_Driver_Write(
        BOOTLOADER_UART_INSTANCE,
        data,
        length
    );

#endif
}


/* ============================================================================
 * Bootloader UART Read
 * ========================================================================== */

bool Bootloader_UART_ReadByte(
    uint8_t *data
)
{
    if (data == NULL)
    {
        return false;
    }

    return UART_Driver_ReadByte(
        BOOTLOADER_UART_INSTANCE,
        data
    );
}