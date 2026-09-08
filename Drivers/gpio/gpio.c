#include "gpio.h"
#include "gpio_hal.h"
#include "board.h"   /* BSP_GPIO_GetPin(GPIO_Name_t) -> GPIO_HAL_Pin_t */

#include <stddef.h>


/* ============================================================
 * Init Table
 * ============================================================
 * Data-driven configuration: one row per logical pin that
 * needs setup at boot. GPIO_Init() walks this table instead
 * of hand-written per-pin init calls.
 *
 * Pins not listed here are simply never configured — add a
 * row when a new logical pin needs to exist.
 * ============================================================ */

typedef struct
{
    GPIO_Name_t     name;
    GPIO_Config_t   config;

} GPIO_InitEntry_t;


#pragma HARDWARE start

static const GPIO_InitEntry_t gpio_init_table[] =
{
    {
        .name   = GPIO_LED,
        .config = {
            .mode               = GPIO_MODE_OUTPUT,
            .pull               = GPIO_PULL_NONE,
            .speed              = GPIO_SPEED_LOW,
            .output_type        = GPIO_OUTPUT_PUSH_PULL,
            .alternate_function = 0U,
            .initial_state      = GPIO_LOW,
        },
    },

    /* GPIO_MOTOR_*, GPIO_ADC_PHASE_* rows added once their
       electrical config (AF/speed for TIM+ADC pins) is fixed. */
};


static const GPIO_HAL_Pin_t gpio_map[GPIO_COUNT] =
{
    [GPIO_LED] = { .port = LED_PORT, .pin = LED_PIN },
};

#pragma HARDWARE END



#define GPIO_INIT_TABLE_COUNT \
    (sizeof(gpio_init_table) / sizeof(gpio_init_table[0]))


/* ============================================================
 * Translation: driver enums -> HAL enums
 * ============================================================
 * This is the ONLY place that knows both enum sets. If the
 * HAL is ever swapped for a different STM32 family (or the
 * HAL enum values are reordered), only this file changes —
 * gpio.h and its callers are untouched.
 * ============================================================ */

static GPIO_HAL_Mode_t ToHalMode(GPIO_Mode_t mode)
{
    switch (mode)
    {
        case GPIO_MODE_INPUT:     return GPIO_HAL_MODE_INPUT;
        case GPIO_MODE_OUTPUT:    return GPIO_HAL_MODE_OUTPUT;
        case GPIO_MODE_ALTERNATE: return GPIO_HAL_MODE_ALTERNATE;
        case GPIO_MODE_ANALOG:    return GPIO_HAL_MODE_ANALOG;
        default:                  return GPIO_HAL_MODE_INPUT;
    }
}

static GPIO_HAL_Pull_t ToHalPull(GPIO_Pull_t pull)
{
    switch (pull)
    {
        case GPIO_PULL_NONE: return GPIO_HAL_PULL_NONE;
        case GPIO_PULL_UP:   return GPIO_HAL_PULL_UP;
        case GPIO_PULL_DOWN: return GPIO_HAL_PULL_DOWN;
        default:             return GPIO_HAL_PULL_NONE;
    }
}

static GPIO_HAL_Speed_t ToHalSpeed(GPIO_Speed_t speed)
{
    switch (speed)
    {
        case GPIO_SPEED_VERY_LOW:  return GPIO_HAL_SPEED_VERY_LOW;
        case GPIO_SPEED_LOW:       return GPIO_HAL_SPEED_LOW;
        case GPIO_SPEED_HIGH:      return GPIO_HAL_SPEED_HIGH;
        case GPIO_SPEED_VERY_HIGH: return GPIO_HAL_SPEED_VERY_HIGH;
        default:                   return GPIO_HAL_SPEED_VERY_LOW;
    }
}

static GPIO_HAL_OutputType_t ToHalOutputType(GPIO_OutputType_t type)
{
    switch (type)
    {
        case GPIO_OUTPUT_PUSH_PULL:  return GPIO_HAL_OUTPUT_PUSH_PULL;
        case GPIO_OUTPUT_OPEN_DRAIN: return GPIO_HAL_OUTPUT_OPEN_DRAIN;
        default:                     return GPIO_HAL_OUTPUT_PUSH_PULL;
    }
}

static GPIO_HAL_State_t ToHalState(GPIO_State_t state)
{
    return (state == GPIO_HIGH) ? GPIO_HAL_HIGH : GPIO_HAL_LOW;
}

/* uint8_t alternate_function passes straight through: neither
   layer assigns it meaning beyond "the AF index the HAL
   register expects", so there's nothing to translate. */


/* ============================================================
 * Internal helper
 * ============================================================ */

 static GPIO_HAL_Pin_t GPIO_GetPin(GPIO_Name_t name)
{
    return gpio_map[name];
}

static void ApplyConfig(GPIO_Name_t name, const GPIO_Config_t *config)
{
    GPIO_HAL_Config_t hal_config;

    hal_config.pin                = GPIO_GetPin(name);
    hal_config.mode               = ToHalMode(config->mode);
    hal_config.output_type        = ToHalOutputType(config->output_type);
    hal_config.pull                = ToHalPull(config->pull);
    hal_config.speed              = ToHalSpeed(config->speed);
    hal_config.alternate_function = (GPIO_HAL_AF_t)config->alternate_function;
    hal_config.initial_state      = ToHalState(config->initial_state);

    GPIO_HAL_Init(&hal_config);
}




/* ============================================================
 * Public API
 * ============================================================ */

void GPIO_Init(void)
{
    for (uint32_t i = 0U; i < GPIO_INIT_TABLE_COUNT; i++)
    {
        ApplyConfig(gpio_init_table[i].name, &gpio_init_table[i].config);
    }
}

void GPIO_Write(GPIO_Name_t name, GPIO_State_t state)
{
    GPIO_HAL_Pin_t pin = GPIO_GetPin(name);
    GPIO_HAL_Write(pin, ToHalState(state));
}

GPIO_State_t GPIO_Read(GPIO_Name_t name)
{
    GPIO_HAL_Pin_t pin = GPIO_GetPin(name);
    return (GPIO_HAL_Read(pin) == GPIO_HAL_HIGH) ? GPIO_HIGH : GPIO_LOW;
}

void GPIO_Toggle(GPIO_Name_t name)
{
    GPIO_HAL_Pin_t pin = GPIO_GetPin(name);
    GPIO_HAL_Toggle(pin);
}

void GPIO_Ioctl(GPIO_Name_t name, GPIO_IoctlCmd_t cmd, const void *arg)
{
    GPIO_HAL_Pin_t pin = GPIO_GetPin(name);

    switch (cmd)
    {
        case GPIO_IOCTL_SET_MODE:
            GPIO_HAL_SetMode(pin, ToHalMode(*(const GPIO_Mode_t *)arg));
            break;

        case GPIO_IOCTL_SET_PULL:
            GPIO_HAL_SetPull(pin, ToHalPull(*(const GPIO_Pull_t *)arg));
            break;

        case GPIO_IOCTL_SET_SPEED:
            GPIO_HAL_SetSpeed(pin, ToHalSpeed(*(const GPIO_Speed_t *)arg));
            break;

        case GPIO_IOCTL_SET_OUTPUT_TYPE:
            GPIO_HAL_SetOutputType(pin, ToHalOutputType(*(const GPIO_OutputType_t *)arg));
            break;

        case GPIO_IOCTL_SET_ALTERNATE_FUNC:
            GPIO_HAL_SetAlternateFunction(pin, (GPIO_HAL_AF_t)(*(const uint8_t *)arg));
            break;

        case GPIO_IOCTL_LOCK:
            GPIO_HAL_LockPin(pin);
            break;

        default:
            /* Unknown command: ignored. Consider a Fault/Debug
               service hook here once Services/Fault exists. */
            break;
    }
}