/**
 * @file    logger.h
 * @brief   Logger - Lightweight Embedded Logging Interface
 *
 * @details
 * Provides a lightweight and reusable logging interface for embedded
 * systems. Supports multiple logging modules, configurable log levels,
 * compile-time and runtime filtering, formatted messages, timestamps,
 * module identification, level tags, optional ANSI colors, and
 * configurable line termination.
 *
 * The logger is designed to remain independent of the system timebase.
 * An external time reference can be supplied through LOG_MainLoop(),
 * allowing the logger to use an existing system timer without requiring
 * a dedicated timer resource.
 *
 * Supported log levels:
 *     DEBUG
 *     INFO
 *     WARNING
 *     ERROR
 *     CRITICAL
 *
 * Supported formatting:
 *     %d  Signed decimal integer
 *     %u  Unsigned decimal integer
 *     %x  Hexadecimal integer
 *     %X  Hexadecimal integer
 *     %b  Binary integer
 *     %c  Character
 *     %s  String
 *     %%  Literal '%' character
 *
 * @author  [Md. Shohanur Rahman]
 * @github  [https://github.com/shohanur00/G4_Core_Lib/tree/main/Services/Logger/Frontend]
 * @linkedIn [https://www.linkedin.com/in/engr-shohanur-rahman-181a69250/]
 * @version [1.0.0]
 * @date    [14/09/2026]
 * @copyright [Copyright All rights reserved]
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Compile-time log level
 * Lower-priority log messages are removed at compile time.
 */
#ifndef LOG_COMPILE_LEVEL
#define LOG_COMPILE_LEVEL   LOG_LEVEL_DEBUG
#endif


/* ============================================================
 * Optional Output Features
 * ============================================================
 *
 * Each feature can be enabled or disabled externally by
 * defining the corresponding macro to 0 or 1 before including
 * this header.
 *
 * Undefined macros use the default values below.
 * ============================================================ */

#pragma USER  start

#ifndef LOG_USE_TIMESTAMP
#define LOG_USE_TIMESTAMP      1U   /* [00012345] external time reference */
#endif

#ifndef LOG_USE_LEVEL_TAG
#define LOG_USE_LEVEL_TAG      1U   /* [INFO] / [WARN] / [ERROR] */
#endif

#ifndef LOG_USE_MODULE_NAME
#define LOG_USE_MODULE_NAME    1U   /* [SYSTEM] / [MOTOR] / ... */
#endif

#ifndef LOG_USE_COLOR
#define LOG_USE_COLOR          1U   /* ANSI color codes for terminal output */
#endif

#ifndef LOG_USE_NEWLINE
#define LOG_USE_NEWLINE        1U   /* Append "\r\n" after each message */
#endif


/* ============================================================
 * Logging Modules
 * ============================================================ */

typedef enum
{
    LOG_MODULE_SYSTEM = 0U,

    /* Add application-specific modules above this entry. */
    /* Example:
     * LOG_MODULE_MOTOR,
     * LOG_MODULE_SENSOR,
     */

    LOG_MODULE_MAX

} LOG_Module_t;


/* ============================================================
 * Logging Levels
 * ============================================================ */

typedef enum
{
    LOG_LEVEL_DEBUG = 0U,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL

} LOG_Level_t;

#pragma USER ends

/* ============================================================
 * Logging Macros
 * ============================================================ */

/*
 * Compile-time filtering:
 *
 * Messages below LOG_COMPILE_LEVEL are removed during
 * preprocessing/compilation and therefore do not result
 * in a LOG_Write() call.
 */

#define LOG_DEBUG(module, ...) \
    ((LOG_LEVEL_DEBUG >= LOG_COMPILE_LEVEL) ? \
        LOG_Write(module, LOG_LEVEL_DEBUG, __VA_ARGS__) : (void)0)

#define LOG_INFO(module, ...) \
    ((LOG_LEVEL_INFO >= LOG_COMPILE_LEVEL) ? \
        LOG_Write(module, LOG_LEVEL_INFO, __VA_ARGS__) : (void)0)

#define LOG_WARNING(module, ...) \
    ((LOG_LEVEL_WARNING >= LOG_COMPILE_LEVEL) ? \
        LOG_Write(module, LOG_LEVEL_WARNING, __VA_ARGS__) : (void)0)

#define LOG_ERROR(module, ...) \
    ((LOG_LEVEL_ERROR >= LOG_COMPILE_LEVEL) ? \
        LOG_Write(module, LOG_LEVEL_ERROR, __VA_ARGS__) : (void)0)

#define LOG_CRITICAL(module, ...) \
    ((LOG_LEVEL_CRITICAL >= LOG_COMPILE_LEVEL) ? \
        LOG_Write(module, LOG_LEVEL_CRITICAL, __VA_ARGS__) : (void)0)


/* ============================================================
 * Logger API
 * ============================================================ */

/**
 * @brief Initialize the logging module.
 *
 * Initializes the logger state and the underlying logging
 * hardware abstraction layer (HAL).
 *
 * This function must be called before using the logger.
 */
void LOG_Init(void);


/**
 * @brief Configure the minimum logging level for a module.
 *
 * Messages with a severity lower than the configured level
 * are filtered out at runtime.
 *
 * @param module
 *     Logging module to configure.
 *
 * @param level
 *     Minimum logging level accepted by the module.
 */
void LOG_SetModuleLevel(
    LOG_Module_t module,
    LOG_Level_t level
);


/**
 * @brief Write a formatted log message.
 *
 * Formats and transmits a log message according to the
 * configured logger features.
 *
 * Supported format specifiers:
 *
 *     %d  Signed decimal integer
 *     %u  Unsigned decimal integer
 *     %x  Hexadecimal integer
 *     %X  Hexadecimal integer
 *     %b  Binary integer
 *     %c  Character
 *     %s  String
 *     %%  Literal '%' character
 *
 * @param module
 *     Module generating the log message.
 *
 * @param level
 *     Severity level of the message.
 *
 * @param format
 *     Format string followed by optional arguments.
 *
 * @note
 * Messages below the configured module level are discarded
 * before formatting.
 */
void LOG_Write(
    LOG_Module_t module,
    LOG_Level_t level,
    const char *format,
    ...
);


/**
 * @brief Update the logger timestamp reference.
 *
 * The logger does not maintain a dedicated timer for
 * timestamp generation. Instead, the application provides
 * the current time reference through this function.
 *
 * This allows the logger to remain independent of the
 * system timekeeping implementation.
 *
 * @param ref_time
 *     Current time reference, typically in milliseconds.
 *
 * @note
 * This function is intended to be called periodically from
 * the application main loop.
 *
 * Example:
 *
 *     LOG_MainLoop(TimeCore_GetTick());
 */
void LOG_MainLoop(uint32_t ref_time);


#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H */

