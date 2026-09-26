/**
 * @file    log_uart.h
 * @brief   UART logging backend interface.
 *
 * @details
 * Provides the interface for initializing the UART logging backend
 * and transmitting log data through the configured UART peripheral.
 *
 * The UART backend is responsible for handling UART-specific
 * initialization and data transmission for the Logger service.
 *
 * @author  [Md. Shohanur Rahman]
 * @github  [https://github.com/shohanur00/G4_Core_Lib/tree/main/Services/Logger/Backends/UART]
 * @linkedIn [https://www.linkedin.com/in/engr-shohanur-rahman-181a69250/]
 * @version [1.0.0]
 * @date    [15/09/2026]
 * @copyright [Copyright All rights reserved]
 */

#ifndef LOG_UART_H

#define LOG_UART_H

#include <stddef.h>

#include <board.h>

#ifdef __cplusplus

extern "C" {

#endif

/**
 * @brief Initialize the UART logging backend.
 *
 * Configures and initializes the UART peripheral used by
 * the Logger for transmitting log messages.
 */
void LOG_UART_Init(void);

/**
 * @brief Write log data through the UART backend.
 *
 * Transmits the specified number of bytes from the provided
 * buffer through the configured UART peripheral.
 *
 * @param[in] buffer Pointer to the log data buffer.
 * @param[in] length Number of bytes to transmit.
 */
void LOG_UART_Write(const char *buffer, size_t length);

#ifdef __cplusplus

}

#endif

#endif /* LOG_UART_H */