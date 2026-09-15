#ifndef BOARD_H
#define BOARD_H

#include "stm32g431xx.h"


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
 * Example:
 *   3U selects USART3.
 * ========================================================================== */

#define LOG_UART_INSTANCE        3U
#define LOG_UART_BAUDRATE        115200U
#define LOG_UART_PARITY          UART_PARITY_NONE
#define LOG_UART_STOP_BITS       UART_STOPBITS_1

#define LOG_UART_TX_PORT         GPIOB
#define LOG_UART_TX_PIN          9U


/* ============================================================================
 * GPIO Configuration
 * ========================================================================== */

#define LED_PORT                GPIOA
#define LED_PIN                 0U


/* ============================================================================
 * Board Information
 * ========================================================================== */

#define BOARD_NAME               "STM32G431CBT6"
#define BOARD_VERSION            "1.0"


#endif /* BOARD_H */