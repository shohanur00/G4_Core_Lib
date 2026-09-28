#ifndef BOOTLOADER_UART_H
#define BOOTLOADER_UART_H

#include <stdint.h>

/* ============================================================================
 * Bootloader UART
 * ========================================================================== */

/**
 * @brief Initialize Bootloader UART.
 *
 * Initializes UART and conditionally enables
 * DMA TX / RX according to BSP configuration.
 */
void Bootloader_UART_Init(void);


/* ============================================================================
 * Bootloader UART Write
 * ========================================================================== */

void Bootloader_UART_Write(
    const uint8_t *data,
    uint32_t       length
);

#endif /* BOOTLOADER_UART_H */