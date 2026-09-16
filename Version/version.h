#pragma once

#include <stdint.h>


/**
 * @file version.h
 * @brief Firmware version logging interface.
 *
 * Provides an interface for logging the current firmware version.
 *
 * @author Md. Shohanur Rahman
 *
 * @see GitHub: github.com/shohanur00/G4_Core_Lib
 * @see LinkedIn: linkedin.com/in/engr-shohanur-rahman-181a69250/
 */


/**
 * @brief Logs the current firmware version.
 *
 * The version is composed of a major version, minor version,
 * and build indicator. The build indicator is automatically
 * generated during the build process.
 *
 * Example:
 *     Firmware Version: 1.0.42
 *
 * @pre LOG_Init() must be called before calling Version_LOG().
 */
void Version_LOG(void);

