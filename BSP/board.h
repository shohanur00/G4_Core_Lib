#ifndef BOARD_H
#define BOARD_H

#include "stm32g431xx.h"
#include "cdefs/cdefs.h"


/* ============================================================================
 * Logger UART Configuration
 * ============================================================================
 *
 * LOG_UART_INSTANCE selects the UART peripheral used by the Logger.
 *
 * Supported values:
 *   1U - USART1
 *   2U - USART2
 *   3U - USART3
 *   4U - UART4
 *   5U - UART5
 *   6U - LPUART1
 *
 * ========================================================================== */

#define LOG_UART_INSTANCE            3U
#define LOG_UART_BAUDRATE            115200U
#define LOG_UART_PARITY              UART_PARITY_NONE
#define LOG_UART_STOP_BITS           UART_STOPBITS_1

#define LOG_UART_TX_PORT             GPIOB
#define LOG_UART_TX_PIN              9U

#define LOG_UART_RX_PORT             GPIOB
#define LOG_UART_RX_PIN              11U


/* ============================================================================
 * Bootloader UART Configuration
 * ============================================================================
 *
 * BOOTLOADER_UART_INSTANCE selects the UART peripheral used by the Bootloader.
 *
 * Supported values:
 *   1U - USART1
 *   2U - USART2
 *   3U - USART3
 *   4U - UART4
 *   5U - UART5
 *   6U - LPUART1
 *
 * ========================================================================== */

#define BOOTLOADER_UART_INSTANCE            3U
#define BOOTLOADER_UART_TX_ENABLE           ENABLE
#define BOOTLOADER_UART_TX_INTERRUPT        DISABLE
#define BOOTLOADER_UART_RX_ENABLE           ENABLE
#define BOOTLOADER_UART_RX_INTERRUPT        ENABLE
#define BOOTLOADER_UART_BAUDRATE            115200U
#define BOOTLOADER_UART_PARITY              UART_PARITY_NONE
#define BOOTLOADER_UART_STOP_BITS           UART_STOPBITS_1

#define BOOTLOADER_UART_TX_PORT             GPIOB
#define BOOTLOADER_UART_TX_PIN              9U

#define BOOTLOADER_UART_RX_PORT             GPIOB
#define BOOTLOADER_UART_RX_PIN              11U


/* ============================================================================
 * Bootloader UART DMA Configuration
 * ============================================================================
 *
 * DMA can be enabled independently for TX and RX.
 *
 * 0U - DMA disabled
 * 1U - DMA enabled
 *
 * ========================================================================== */

#define BOOTLOADER_UART_DMA_TX_ENABLE    DISABLE
#define BOOTLOADER_UART_DMA_RX_ENABLE    DISABLE

#define BOOTLOADER_UART_DMA_TX_CHANNEL   1U
#define BOOTLOADER_UART_DMA_RX_CHANNEL   2U

/* DMAMUX request selection */
#define BOOTLOADER_UART_DMA_TX_REQUEST   DISABLE
#define BOOTLOADER_UART_DMA_RX_REQUEST   DISABLE


/* ============================================================================
 * GPIO Configuration
 * ========================================================================== */

#define LED_PORT                    GPIOA
#define LED_PIN                     0U


/* ============================================================================
 * Board Information
 * ========================================================================== */

#define BOARD_NAME                  "STM32G431CBT6"
#define BOARD_VERSION               "1.0"


#endif /* BOARD_H */