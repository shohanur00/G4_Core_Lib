#ifndef BOOTLOADER_APP_H
#define BOOTLOADER_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Project Information
 * ========================================================================== */

/**
 * @file    bootloader_app.h
 * @brief   Bootloader application interface.
 *
 * @author  Engr. Shohanur Rahman
 *
 * @project G4_Core_Lib
 * @module  STM32 Bootloader
 *
 * @details
 * This module provides the application-level interface for
 * bootloader initialization, execution, and deinitialization.
 *
 * --------------------------------------------------------------------------
 * Author
 * --------------------------------------------------------------------------
 * Engr. Shohanur Rahman
 * Embedded Software Engineer
 * Research & Innovation
 *
 * LinkedIn:
 * https://www.linkedin.com/in/engr-shohanur-rahman-181a69250/
 *
 * GitHub:
 * https://github.com/shohanur00
 *
 * Project:
 * https://github.com/shohanur00/G4_Core_Lib
 *
 * --------------------------------------------------------------------------
 * Copyright
 * --------------------------------------------------------------------------
 * Copyright (c) 2026 Engr. Shohanur Rahman
 */

/* ============================================================================
 * Bootloader Application
 * ========================================================================== */

/**
 * @brief Deinitialize the bootloader application.
 *
 * Releases or resets application-level resources before leaving
 * the bootloader environment.
 */
void Bootloader_App_Deinit(void);

/**
 * @brief Initialize the bootloader application.
 *
 * Performs bootloader-level initialization required before entering
 * the main bootloader execution loop.
 */
void Bootloader_App_Setup(void);

/**
 * @brief Execute one iteration of the bootloader application.
 *
 * Handles the main bootloader application processing and state
 * machine execution.
 */
void Bootloader_App_Loop(void);

#ifdef __cplusplus
}
#endif

#endif /* BOOTLOADER_APP_H */