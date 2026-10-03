#include "bl_transport.h"
#include "board.h"

#if BOOTLOADER_USE_UART == ENABLE

#include "transport/uart/bootloader_uart.h"

#endif




/* ============================================================================
 * Transport Initialization
 * ========================================================================== */

void BL_Transport_Init(void)
{
#if (BOOTLOADER_USE_UART == 1U)

    Bootloader_UART_Init();

#endif
}


/* ============================================================================
 * Transport Data Available
 * ========================================================================== */

uint16_t BL_Transport_DataAvailable(void)
{
#if (BOOTLOADER_USE_UART == 1U)

    return Bootloader_UART_DataAvailable();

#else

    return 0U;

#endif
}


/* ============================================================================
 * Transport Read Byte
 * ========================================================================== */

bool BL_Transport_ReadByte(
    uint8_t *byte
)
{
    if (byte == NULL)
    {
        return false;
    }

#if (BOOTLOADER_USE_UART == 1U)

    return Bootloader_UART_ReadByte(byte);

#else

    return false;

#endif
}


/* ============================================================================
 * Transport Write
 * ========================================================================== */

bool BL_Transport_Write(
    const uint8_t *data,
    uint16_t        length
)
{
    if ((data == NULL) ||
        (length == 0U))
    {
        return false;
    }

#if (BOOTLOADER_USE_UART == 1U)

    Bootloader_UART_Write(
        data,
        length
    );

    return true;

#else

    return false;

#endif
}