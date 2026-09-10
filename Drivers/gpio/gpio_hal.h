/**
 * @file gpio_hal.h
 * @brief GPIO Hardware Abstraction Layer (HAL) - Low-level GPIO control interface
 * @details Provides a portable GPIO configuration and control interface for the STM32G4 
 *          microcontroller series. Abstracts hardware-specific register manipulation and 
 *          clock management. Supports multiple GPIO modes (input, output, alternate function, 
 *          analog), output types (push-pull, open-drain), pull configurations, output speeds,
 *          and alternate function selection. Includes automatic port clock enablement and
 *          glitch-free initialization with configurable initial pin states.
 * 
 * @note GPIO_HAL_Init() applies initial_state to the output register BEFORE switching 
 *       MODER to output mode, preventing electrical glitches on sensitive pins such as 
 *       motor gate-driver pins during mode transitions.
 * @github [https://github.com/shohanur00/G4_Core_Lib/tree/main/Drivers/gpio]
 * @author [Md. Shohanur Rahman]
 * @version 1.0.0
 * @date [09/09/2026]
 * @copyright [Copyright All rights reserved]
 * 
 * @target STM32G431xx microcontroller
 * @dependencies stm32g431xx.h, stdint.h, stddef.h
 */
#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#include <stdint.h>
#include "stm32g431xx.h"
#include <stddef.h>


/* ============================================================
 * GPIO HAL - Pin Handle
 * ============================================================
 * Bundles port + pin so callers (gpio.c, board_gpio.c tables)
 * pass a single value instead of two parameters everywhere.
 * ============================================================ */

typedef struct
{
    GPIO_TypeDef *port;
    uint8_t       pin;

} GPIO_HAL_Pin_t;


/* ============================================================
 * GPIO HAL - Modes
 * ============================================================ */

typedef enum
{
    GPIO_HAL_MODE_INPUT = 0U,
    GPIO_HAL_MODE_OUTPUT,
    GPIO_HAL_MODE_ALTERNATE,
    GPIO_HAL_MODE_ANALOG

} GPIO_HAL_Mode_t;


/* ============================================================
 * GPIO HAL - Output Type
 * ============================================================ */

typedef enum
{
    GPIO_HAL_OUTPUT_PUSH_PULL = 0U,
    GPIO_HAL_OUTPUT_OPEN_DRAIN

} GPIO_HAL_OutputType_t;


/* ============================================================
 * GPIO HAL - Pull Configuration
 * ============================================================ */

typedef enum
{
    GPIO_HAL_PULL_NONE = 0U,
    GPIO_HAL_PULL_UP,
    GPIO_HAL_PULL_DOWN

} GPIO_HAL_Pull_t;


/* ============================================================
 * GPIO HAL - Output Speed
 * ============================================================ */

typedef enum
{
    GPIO_HAL_SPEED_VERY_LOW = 0U,
    GPIO_HAL_SPEED_LOW,
    GPIO_HAL_SPEED_HIGH,
    GPIO_HAL_SPEED_VERY_HIGH

} GPIO_HAL_Speed_t;


/* ============================================================
 * GPIO HAL - Pin State
 * ============================================================ */

typedef enum
{
    GPIO_HAL_LOW = 0U,
    GPIO_HAL_HIGH

} GPIO_HAL_State_t;


/* ============================================================
 * GPIO HAL - Alternate Function
 * ============================================================ */

typedef enum
{
    GPIO_HAL_AF0 = 0U,
    GPIO_HAL_AF1,
    GPIO_HAL_AF2,
    GPIO_HAL_AF3,
    GPIO_HAL_AF4,
    GPIO_HAL_AF5,
    GPIO_HAL_AF6,
    GPIO_HAL_AF7,
    GPIO_HAL_AF8,
    GPIO_HAL_AF9,
    GPIO_HAL_AF10,
    GPIO_HAL_AF11,
    GPIO_HAL_AF12,
    GPIO_HAL_AF13,
    GPIO_HAL_AF14,
    GPIO_HAL_AF15

} GPIO_HAL_AF_t;


/* ============================================================
 * GPIO HAL - Configuration Structure
 * ============================================================
 * NOTE: GPIO_HAL_Init() applies initial_state to the output
 * register BEFORE switching MODER to output, to avoid a
 * glitch on the pin during the mode transition. This matters
 * for motor gate-driver pins in particular.
 * ============================================================ */

typedef struct
{
    GPIO_HAL_Pin_t             pin;

    GPIO_HAL_Mode_t            mode;
    GPIO_HAL_OutputType_t      output_type;
    GPIO_HAL_Pull_t            pull;
    GPIO_HAL_Speed_t           speed;
    GPIO_HAL_AF_t              alternate_function;

    GPIO_HAL_State_t           initial_state;

} GPIO_HAL_Config_t;


/* ============================================================
 * Initialization
 * ============================================================
 * Enables the port's clock internally — caller does not need
 * to call a separate clock-enable step. Safe to call multiple
 * times for pins sharing the same port.
 * ============================================================ */

void GPIO_HAL_Init(const GPIO_HAL_Config_t *config);


/* ============================================================
 * Digital Output
 * ============================================================ */

void GPIO_HAL_Write(GPIO_HAL_Pin_t pin, GPIO_HAL_State_t state);

void GPIO_HAL_Toggle(GPIO_HAL_Pin_t pin);


/* ============================================================
 * Digital Input
 * ============================================================ */

GPIO_HAL_State_t GPIO_HAL_Read(GPIO_HAL_Pin_t pin);


/* ============================================================
 * Pin Configuration (used by GPIO_Ioctl in the driver layer)
 * ============================================================ */

void GPIO_HAL_SetMode(GPIO_HAL_Pin_t pin, GPIO_HAL_Mode_t mode);

void GPIO_HAL_SetOutputType(GPIO_HAL_Pin_t pin, GPIO_HAL_OutputType_t type);

void GPIO_HAL_SetPull(GPIO_HAL_Pin_t pin, GPIO_HAL_Pull_t pull);

void GPIO_HAL_SetSpeed(GPIO_HAL_Pin_t pin, GPIO_HAL_Speed_t speed);

void GPIO_HAL_SetAlternateFunction(GPIO_HAL_Pin_t pin, GPIO_HAL_AF_t af);


/* ============================================================
 * Lock
 * ============================================================ */

void GPIO_HAL_LockPin(GPIO_HAL_Pin_t pin);

#endif /* GPIO_HAL_H */