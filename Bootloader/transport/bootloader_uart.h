#ifndef BOOTLOADER_UART_H
#define BOOTLOADER_UART_H

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

#endif /* BOOTLOADER_UART_H */