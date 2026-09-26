/**
 * @file    logger_hal.h
 * @brief   Logger Hardware Abstraction Layer interface.
 *
 * @details
 * Provides the hardware abstraction interface between the Logger core
 * and its output backends.
 *
 * The Logger HAL allows the same logging interface to forward log data
 * to different hardware or software backends, such as UART, Flash, USB,
 * SEGGER RTT, RAM, LED indication, or a user-defined custom backend.
 *
 * Backend selection is controlled through the LOG_HAL_USE_* configuration
 * macros. Multiple backends may be enabled simultaneously.
 *
 * The Logger core remains independent of the underlying output hardware.
 * Backend-specific initialization and data transmission are handled by
 * the corresponding backend implementation.
 *
 * @author  [Md. Shohanur Rahman]
 * @github  [https://github.com/shohanur00/G4_Core_Lib/tree/main/Services/Logger/Core]
 * @linkedIn [https://www.linkedin.com/in/engr-shohanur-rahman-181a69250/]
 * @version [1.0.0]
 * @date    [15/09/2026]
 * @copyright [Copyright All rights reserved]
 */

#ifndef LOGGER_HAL_H
#define LOGGER_HAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ============================================================
 * Logger Output Configuration
 * ============================================================ */

/**
 * @brief Enable or disable the UART logging backend.
 *
 * When enabled, log messages are forwarded to the UART backend.
 *
 * @note Set to 1U to enable or 0U to disable.
 */
#define LOG_HAL_USE_UART        1U


/**
 * @brief Enable or disable the Flash logging backend.
 *
 * When enabled, log messages are stored in non-volatile Flash
 * memory through the Flash backend.
 *
 * @note Set to 1U to enable or 0U to disable.
 */
#define LOG_HAL_USE_FLASH       0U


/**
 * @brief Enable or disable the USB logging backend.
 *
 * When enabled, log messages are forwarded through the USB
 * logging backend.
 *
 * @note Set to 1U to enable or 0U to disable.
 */
#define LOG_HAL_USE_USB         0U


/**
 * @brief Enable or disable the SEGGER RTT logging backend.
 *
 * When enabled, log messages are forwarded through the
 * SEGGER RTT backend.
 *
 * @note Set to 1U to enable or 0U to disable.
 */
#define LOG_HAL_USE_RTT         0U


/**
 * @brief Enable or disable the RAM logging backend.
 *
 * When enabled, log messages are stored in a RAM-based
 * logging buffer.
 *
 * @note Set to 1U to enable or 0U to disable.
 */
#define LOG_HAL_USE_RAM         0U


/**
 * @brief Enable or disable the LED blink logging backend.
 *
 * When enabled, logging events can be represented through
 * LED blinking patterns.
 *
 * @note Set to 1U to enable or 0U to disable.
 */
#define LOG_HAL_USE_BLINK       0U


/**
 * @brief Enable or disable the custom logging backend.
 *
 * When enabled, log messages are forwarded to a user-defined
 * custom logging backend.
 *
 * @note Set to 1U to enable or 0U to disable.
 */
#define LOG_HAL_USE_CUSTOM      0U


/* ============================================================
 * Logger HAL API
 * ============================================================ */

/**
 * @brief Initialize the Logger Hardware Abstraction Layer.
 *
 * Initializes all logging backends enabled through the
 * LOG_HAL_USE_* configuration macros.
 *
 * Each enabled backend is responsible for initializing its
 * own hardware or software resources.
 *
 * @note This function should be called once before using
 *       the Logger.
 */
void LOG_HAL_Init(void);


/**
 * @brief Write log data to all enabled logging backends.
 *
 * The Logger HAL forwards the provided log buffer to every
 * backend enabled through the LOG_HAL_USE_* configuration
 * macros.
 *
 * Multiple backends may be enabled simultaneously. In that
 * case, the same log data is forwarded to each enabled backend.
 *
 * @param[in] buffer
 *     Pointer to the log data buffer.
 *
 * @param[in] length
 *     Number of bytes to write from the buffer.
 *
 * @note The buffer does not need to be null-terminated because
 *       the length parameter explicitly specifies the number
 *       of bytes to write.
 *
 * @note The behavior of the function, such as blocking,
 *       buffering, or DMA-based transmission, depends on
 *       the enabled backend implementations.
 */
void LOG_HAL_Write(
    const char *buffer,
    size_t length
);


#ifdef __cplusplus
}
#endif

#endif /* LOGGER_HAL_H */