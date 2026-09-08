#include "gpio_hal.h"

/* ============================================================
 * Private Helpers
 * ============================================================ */

static uint32_t GPIO_HAL_PinMask(uint8_t pin)
{
    return (1UL << pin);
}

/* Internal-only Set/Reset — not part of the public API.
   GPIO_HAL_Write() is the single public entry point for
   digital output; these exist purely so Toggle() can flip
   a single bit without re-deriving state from Write(). */
static void GPIO_HAL_Set(GPIO_HAL_Pin_t pin)
{
    pin.port->BSRR = GPIO_HAL_PinMask(pin.pin);
}

static void GPIO_HAL_Reset(GPIO_HAL_Pin_t pin)
{
    pin.port->BSRR = GPIO_HAL_PinMask(pin.pin) << 16U;
}


/* ============================================================
 * Clock
 * ============================================================
 * Internal-only: called from GPIO_HAL_Init(). Not declared in
 * gpio_hal.h, so callers never need to remember to enable a
 * port's clock themselves. Safe to call repeatedly for pins
 * sharing the same port — RCC bits are idempotent to re-set.
 * ============================================================ */

static void GPIO_HAL_EnableClock(GPIO_TypeDef *port)
{
    if (port == GPIOA)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    }
    else if (port == GPIOB)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    }
    else if (port == GPIOC)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    }
    else if (port == GPIOD)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
    }
    else if (port == GPIOE)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
    }
    else if (port == GPIOF)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
    }
}


/* ============================================================
 * Mode
 *
 * MODER:
 *
 * 00 = Input
 * 01 = Output
 * 10 = Alternate Function
 * 11 = Analog
 * ============================================================ */

void GPIO_HAL_SetMode(GPIO_HAL_Pin_t pin, GPIO_HAL_Mode_t mode)
{
    uint32_t position = pin.pin * 2U;

    pin.port->MODER &= ~(3UL << position);
    pin.port->MODER |= ((uint32_t)mode << position);
}


/* ============================================================
 * Output Type
 *
 * OTYPER:
 *
 * 0 = Push-Pull
 * 1 = Open-Drain
 * ============================================================ */

void GPIO_HAL_SetOutputType(GPIO_HAL_Pin_t pin, GPIO_HAL_OutputType_t type)
{
    if (type == GPIO_HAL_OUTPUT_OPEN_DRAIN)
    {
        pin.port->OTYPER |= GPIO_HAL_PinMask(pin.pin);
    }
    else
    {
        pin.port->OTYPER &= ~GPIO_HAL_PinMask(pin.pin);
    }
}


/* ============================================================
 * Pull Configuration
 *
 * PUPDR:
 *
 * 00 = No Pull
 * 01 = Pull-Up
 * 10 = Pull-Down
 * 11 = Reserved
 * ============================================================ */

void GPIO_HAL_SetPull(GPIO_HAL_Pin_t pin, GPIO_HAL_Pull_t pull)
{
    uint32_t position = pin.pin * 2U;

    pin.port->PUPDR &= ~(3UL << position);
    pin.port->PUPDR |= ((uint32_t)pull << position);
}


/* ============================================================
 * Output Speed
 *
 * OSPEEDR:
 *
 * 00 = Very Low
 * 01 = Low
 * 10 = High
 * 11 = Very High
 * ============================================================ */

void GPIO_HAL_SetSpeed(GPIO_HAL_Pin_t pin, GPIO_HAL_Speed_t speed)
{
    uint32_t position = pin.pin * 2U;

    pin.port->OSPEEDR &= ~(3UL << position);
    pin.port->OSPEEDR |= ((uint32_t)speed << position);
}


/* ============================================================
 * Alternate Function
 *
 * AFR[0] -> Pin 0-7
 * AFR[1] -> Pin 8-15
 *
 * Each pin uses 4 bits.
 * ============================================================ */

void GPIO_HAL_SetAlternateFunction(GPIO_HAL_Pin_t pin, GPIO_HAL_AF_t af)
{
    uint8_t index    = pin.pin / 8U;
    uint8_t position = (pin.pin % 8U) * 4U;

    pin.port->AFR[index] &= ~(0xFUL << position);
    pin.port->AFR[index] |= ((uint32_t)af << position);
}


/* ============================================================
 * Write
 *
 * BSRR:
 *
 * Bits 0-15  -> Set
 * Bits 16-31 -> Reset
 * ============================================================ */

void GPIO_HAL_Write(GPIO_HAL_Pin_t pin, GPIO_HAL_State_t state)
{
    if (state == GPIO_HAL_HIGH)
    {
        GPIO_HAL_Set(pin);
    }
    else
    {
        GPIO_HAL_Reset(pin);
    }
}


/* ============================================================
 * Toggle
 * ============================================================ */

void GPIO_HAL_Toggle(GPIO_HAL_Pin_t pin)
{
    if (pin.port->ODR & GPIO_HAL_PinMask(pin.pin))
    {
        GPIO_HAL_Reset(pin);
    }
    else
    {
        GPIO_HAL_Set(pin);
    }
}


/* ============================================================
 * Read
 * ============================================================ */

GPIO_HAL_State_t GPIO_HAL_Read(GPIO_HAL_Pin_t pin)
{
    if (pin.port->IDR & GPIO_HAL_PinMask(pin.pin))
    {
        return GPIO_HAL_HIGH;
    }

    return GPIO_HAL_LOW;
}


/* ============================================================
 * Complete GPIO Initialization
 * ============================================================
 * Order matters here to avoid glitches:
 *   1. Enable clock (must happen before touching any register)
 *   2. Output type / pull / speed / AF — pin is still in
 *      whatever mode it reset into, so none of this drives
 *      the pad yet.
 *   3. Initial output state, written to BSRR — still before
 *      switching MODER, so the output driver is pre-loaded
 *      with the correct level before it goes live.
 *   4. Mode — switching MODER last means the instant the pin
 *      becomes an output (or AF), it is already electrically
 *      correct. This matters most for gate-driver pins.
 * ============================================================ */

void GPIO_HAL_Init(const GPIO_HAL_Config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    GPIO_HAL_EnableClock(config->pin.port);

    GPIO_HAL_SetOutputType(config->pin, config->output_type);
    GPIO_HAL_SetPull(config->pin, config->pull);
    GPIO_HAL_SetSpeed(config->pin, config->speed);

    if (config->mode == GPIO_HAL_MODE_ALTERNATE)
    {
        GPIO_HAL_SetAlternateFunction(config->pin, config->alternate_function);
    }

    if (config->mode == GPIO_HAL_MODE_OUTPUT)
    {
        GPIO_HAL_Write(config->pin, config->initial_state);
    }

    GPIO_HAL_SetMode(config->pin, config->mode);
}


/* ============================================================
 * GPIO Lock
 * ============================================================
 * LCKR sequence (per reference manual):
 *   1. Write 1 + pin
 *   2. Write 0 + pin
 *   3. Write 1 + pin
 *   4. Read
 *   5. Read
 * ============================================================ */

void GPIO_HAL_LockPin(GPIO_HAL_Pin_t pin)
{
    uint32_t lock_mask = GPIO_HAL_PinMask(pin.pin);

    pin.port->LCKR = lock_mask | GPIO_LCKR_LCKK;
    pin.port->LCKR = lock_mask;
    pin.port->LCKR = lock_mask | GPIO_LCKR_LCKK;

    (void)pin.port->LCKR;
    (void)pin.port->LCKR;
}