/**
 * @file gpio.h
 * @brief GPIO - General Purpose Input/Output Driver
 * @details Provides a hardware-abstraction layer for GPIO pin management
 *          across different MCU families. Supports pin configuration,
 *          state control, and runtime reconfiguration through ioctl commands.
 *          All physical port/pin mappings are isolated in the BSP layer
 *          (board_gpio.h/.c), keeping application code MCU-independent.
 * 
 * @section Features
 *   - Logical pin naming (application uses symbolic names only)
 *   - Flexible pin configuration (mode, pull, speed, output type, alternate function)
 *   - Runtime pin reconfiguration via ioctl interface
 *   - Fast hot-path operations (read, write, toggle) via dedicated functions
 *   - Support for input, output, alternate, and analog modes
 *   - Configurable pull resistors and output drive characteristics
 *   - Pin locking capability for write-protection
 * 
 * @section Pin Configuration
 *   Pin behavior is defined through GPIO_Config_t structure containing:
 *   - Mode: input, output, alternate function, or analog
 *   - Pull configuration: none, up, or down
 *   - Output speed: very low to very high
 *   - Output type: push-pull or open-drain
 *   - Alternate function: raw index (0-15) for MCU-specific muxing
 *   - Initial state: LOW or HIGH
 * 
 * @author [Md. Shohanur Rahman]
 * @version 1.0.0
 * @date [09/09/2026]
 * @copyright [Copyright All rights reserved]
 */


 
#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>


/* ============================================================
 * GPIO - Logical Pin Names
 * ============================================================
 * Application code refers to pins only by these names. No
 * physical port/pin or STM32 detail is ever exposed here —
 * that mapping lives in BSP (board.h/.c).
 * ============================================================ */

#pragma HARDWARE start

typedef enum
{
    GPIO_LED,


    GPIO_COUNT

} GPIO_Name_t;

#pragma HARDWARE END

/* ============================================================
 * GPIO - Pin State
 * ============================================================ */

typedef enum
{
    GPIO_LOW = 0U,
    GPIO_HIGH

} GPIO_State_t;


/* ============================================================
 * GPIO - Mode
 * ============================================================ */

typedef enum
{
    GPIO_MODE_INPUT = 0U,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_ALTERNATE,
    GPIO_MODE_ANALOG

} GPIO_Mode_t;


/* ============================================================
 * GPIO - Pull Configuration
 * ============================================================ */

typedef enum
{
    GPIO_PULL_NONE = 0U,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN

} GPIO_Pull_t;


/* ============================================================
 * GPIO - Output Speed
 * ============================================================ */

typedef enum
{
    GPIO_SPEED_VERY_LOW = 0U,
    GPIO_SPEED_LOW,
    GPIO_SPEED_HIGH,
    GPIO_SPEED_VERY_HIGH

} GPIO_Speed_t;


/* ============================================================
 * GPIO - Output Type
 * ============================================================ */

typedef enum
{
    GPIO_OUTPUT_PUSH_PULL = 0U,
    GPIO_OUTPUT_OPEN_DRAIN

} GPIO_OutputType_t;


/* ============================================================
 * GPIO - Configuration Structure
 * ============================================================
 * MCU-independent from the driver's point of view. Used both
 * for the internal init table (gpio.c) and for GPIO_Ioctl
 * reconfiguration at runtime.
 *
 * alternate_function is a raw index (0-15 on STM32), not an
 * enum: the driver layer treats it as an opaque mux selector
 * and passes it straight through to the HAL, which is the
 * only layer that knows what each AF number actually connects
 * to on this MCU family. Mirroring GPIO_HAL_AF_t here would
 * just be a second enum you'd have to keep in sync with the
 * first for no behavioral benefit.
 * ============================================================ */

typedef struct
{
    GPIO_Mode_t         mode;
    GPIO_Pull_t         pull;
    GPIO_Speed_t        speed;
    GPIO_OutputType_t   output_type;
    uint8_t             alternate_function;
    GPIO_State_t        initial_state;

} GPIO_Config_t;


/* ============================================================
 * GPIO - Ioctl Commands
 * ============================================================
 * Covers the long tail of infrequent, structurally different
 * control operations. Frequent hot-path operations (read,
 * write, toggle) are NOT here — they are dedicated functions
 * below for type safety and call-site clarity.
 *
 * arg meaning per command:
 *   GPIO_IOCTL_SET_MODE            -> const GPIO_Mode_t*
 *   GPIO_IOCTL_SET_PULL            -> const GPIO_Pull_t*
 *   GPIO_IOCTL_SET_SPEED           -> const GPIO_Speed_t*
 *   GPIO_IOCTL_SET_OUTPUT_TYPE     -> const GPIO_OutputType_t*
 *   GPIO_IOCTL_SET_ALTERNATE_FUNC  -> const uint8_t*
 *   GPIO_IOCTL_LOCK                -> NULL (unused)
 * ============================================================ */

typedef enum
{
    GPIO_IOCTL_SET_MODE = 0U,
    GPIO_IOCTL_SET_PULL,
    GPIO_IOCTL_SET_SPEED,
    GPIO_IOCTL_SET_OUTPUT_TYPE,
    GPIO_IOCTL_SET_ALTERNATE_FUNC,
    GPIO_IOCTL_LOCK

} GPIO_IoctlCmd_t;


/* ============================================================
 * Public API
 * ============================================================
 * GPIO_Init() configures every pin listed in the internal
 * init table (gpio.c) in one call, using board mapping from
 * BSP. Application code never configures a pin individually
 * at startup.
 * ============================================================ */

void GPIO_Init(void);

void GPIO_Write(GPIO_Name_t name, GPIO_State_t state);

GPIO_State_t GPIO_Read(GPIO_Name_t name);

void GPIO_Toggle(GPIO_Name_t name);

void GPIO_Ioctl(GPIO_Name_t name, GPIO_IoctlCmd_t cmd, const void *arg);

#endif /* GPIO_H */