#ifndef LOGGER_HAL_H
#define LOGGER_HAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif




/* ============================================================
 * Logger Output Configuration
 * ============================================================ */

#define LOG_HAL_USE_UART        1U
#define LOG_HAL_USE_FLASH       0U
#define LOG_HAL_USE_USB         0U
#define LOG_HAL_USE_RTT         0U
#define LOG_HAL_USE_RAM         0U
#define LOG_HAL_USE_BLINK       0U
#define LOG_HAL_USE_CUSTOM      0U



/**
 * @brief Initialize the logger hardware abstraction layer.
 *
 * Initializes all enabled logging backends.
 */
void LOG_HAL_Init(void);


/**
 * @brief Write log data to all enabled logging backends.
 *
 * The logger HAL forwards the provided log buffer to each
 * backend enabled by the project configuration.
 *
 * @param buffer
 *     Pointer to the log data.
 *
 * @param length
 *     Number of bytes to write.
 */
void LOG_HAL_Write(
    const char *buffer,
    size_t length
);


#ifdef __cplusplus
}
#endif

#endif /* LOGGER_HAL_H */