<div align="center">

# GPIO Driver

![Status](https://img.shields.io/badge/status-active-brightgreen)
![MCU](https://img.shields.io/badge/MCU-STM32G431-blue)
![Language](https://img.shields.io/badge/language-C-orange)
![License](https://img.shields.io/badge/license-All%20Rights%20Reserved-lightgrey)

</div>

A lightweight and reusable GPIO driver for STM32G4, designed with a layered architecture that keeps application-level GPIO usage independent from MCU-specific hardware details.

The driver provides:

* Logical GPIO naming
* Configurable GPIO modes
* Pull-up / pull-down configuration
* Output speed configuration
* Push-pull / open-drain output
* Alternate function selection
* Runtime GPIO reconfiguration
* Read / Write / Toggle operations
* GPIO pin locking
* Data-driven initialization
* Hardware-specific configuration through a dedicated `#pragma HARDWARE` section

---

## Directory Structure

```text
Drivers/
└── gpio/
    ├── gpio.h
    ├── gpio.c
    ├── gpio_hal.h
    └── gpio_hal.c
```

### Layering

```text
Application
     │
     ▼
   gpio.h
     │
     ▼
   gpio.c
     │
     ▼
 gpio_hal.h
     │
     ▼
 gpio_hal.c
     │
     ▼
STM32G4 GPIO Registers
```

The driver layer works with logical GPIO names, while the HAL handles STM32G4-specific register operations.

---

## Logical GPIO Naming

Application code does not directly use MCU port/pin numbers.

Instead, logical names are defined through `GPIO_Name_t`:

```c
typedef enum
{
    GPIO_LED,
    LOG_UART_TX,

    GPIO_COUNT

} GPIO_Name_t;
```

For example:

```c
GPIO_Write(GPIO_LED, GPIO_HIGH);
```

The application does not need to know whether `GPIO_LED` is connected to `PA0`, `PB5`, or another physical pin.

This keeps application-level code independent from the actual PCB pin assignment.

---

# Hardware Configuration

Hardware-dependent GPIO information is intentionally separated from the generic driver logic.

The following sections are marked using:

```c
#pragma USER start

/* Hardware-specific configuration */

#pragma USER END
```

These sections identify the parts of the GPIO driver that normally need to be modified when adapting the library to a different board or hardware design.

---

## 1. Logical Pin Definition

The logical GPIO names are located inside the `HARDWARE` section of `gpio.h`:

```c
#pragma USER start

typedef enum
{
    GPIO_LED,
    LOG_UART_TX,

    GPIO_COUNT

} GPIO_Name_t;

#pragma USER END
```

When adding a new hardware-controlled GPIO, add its logical name here.

For example:

```c
#pragma USER start

typedef enum
{
    GPIO_LED,
    LOG_UART_TX,
    GPIO_MOTOR_EN,
    GPIO_FAN_EN,

    GPIO_COUNT

} GPIO_Name_t;

#pragma USER END
```

The application can then use:

```c
GPIO_Write(GPIO_MOTOR_EN, GPIO_HIGH);
```

without knowing the physical MCU pin.

---

## 2. GPIO Initialization Table

Hardware-specific GPIO configuration is defined in `gpio.c` using a data-driven initialization table.

The table is also enclosed within the `HARDWARE` section:

```c
#pragma USER start

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

    {
        .name   = LOG_UART_TX,
        .config = {
            .mode               = GPIO_MODE_ALTERNATE,
            .pull               = GPIO_PULL_UP,
            .speed              = GPIO_SPEED_HIGH,
            .output_type        = GPIO_OUTPUT_PUSH_PULL,
            .alternate_function = 7U,
            .initial_state      = GPIO_LOW,
        },
    },
};

#pragma USER END
```

Each row represents one logical GPIO and its startup configuration.

The driver automatically processes this table when:

```c
GPIO_Init();
```

is called.

This avoids writing individual initialization functions for every GPIO.

---

## 3. Physical GPIO Mapping

The logical GPIO names are mapped to actual MCU ports and pins inside the hardware section of `gpio.c`:

```c
#pragma USER start

static const GPIO_HAL_Pin_t gpio_map[GPIO_COUNT] =
{
    [GPIO_LED] =
    {
        .port = LED_PORT,
        .pin  = LED_PIN
    },

    [LOG_UART_TX] =
    {
        .port = LOG_UART_TX_PORT,
        .pin  = LOG_UART_TX_PIN
    },
};

#pragma USER END
```

The actual physical mapping is provided by `board.h`.

For example:

```c
#define LED_PORT          GPIOA
#define LED_PIN           0U

#define LOG_UART_TX_PORT  GPIOB
#define LOG_UART_TX_PIN   9U
```

This gives the following separation:

```text
Logical Name
     │
     ▼
GPIO_LED
     │
     ▼
gpio_map[]
     │
     ▼
LED_PORT / LED_PIN
     │
     ▼
Physical MCU Pin
```

---

# Adding a New GPIO

Adding a new GPIO normally requires changes only in the hardware-specific sections.

### Step 1 — Add logical name

In `gpio.h`:

```c
#pragma USER start

typedef enum
{
    GPIO_LED,
    LOG_UART_TX,
    GPIO_FAN_EN,

    GPIO_COUNT

} GPIO_Name_t;

#pragma USER END
```

### Step 2 — Add physical mapping

In `board.h`:

```c
#define FAN_EN_PORT    GPIOB
#define FAN_EN_PIN     5U
```

### Step 3 — Add initialization configuration

In `gpio.c`:

```c
#pragma USER start

static const GPIO_InitEntry_t gpio_init_table[] =
{
    /* Existing entries */

    {
        .name   = GPIO_FAN_EN,
        .config = {
            .mode               = GPIO_MODE_OUTPUT,
            .pull               = GPIO_PULL_NONE,
            .speed              = GPIO_SPEED_LOW,
            .output_type        = GPIO_OUTPUT_PUSH_PULL,
            .alternate_function = 0U,
            .initial_state      = GPIO_LOW,
        },
    },
};

#pragma USER END
```

### Step 4 — Add the logical-to-physical mapping

```c
#pragma USER start

static const GPIO_HAL_Pin_t gpio_map[GPIO_COUNT] =
{
    /* Existing mappings */

    [GPIO_FAN_EN] =
    {
        .port = FAN_EN_PORT,
        .pin  = FAN_EN_PIN
    },
};

#pragma USER END
```

The application can now simply use:

```c
GPIO_Write(GPIO_FAN_EN, GPIO_HIGH);
```

---

# GPIO Configuration

Each GPIO can be configured using `GPIO_Config_t`:

```c
typedef struct
{
    GPIO_Mode_t         mode;
    GPIO_Pull_t         pull;
    GPIO_Speed_t        speed;
    GPIO_OutputType_t   output_type;
    uint8_t             alternate_function;
    GPIO_State_t        initial_state;

} GPIO_Config_t;
```

### Supported Modes

```text
GPIO_MODE_INPUT
GPIO_MODE_OUTPUT
GPIO_MODE_ALTERNATE
GPIO_MODE_ANALOG
```

### Pull Configuration

```text
GPIO_PULL_NONE
GPIO_PULL_UP
GPIO_PULL_DOWN
```

### Output Speed

```text
GPIO_SPEED_VERY_LOW
GPIO_SPEED_LOW
GPIO_SPEED_HIGH
GPIO_SPEED_VERY_HIGH
```

### Output Type

```text
GPIO_OUTPUT_PUSH_PULL
GPIO_OUTPUT_OPEN_DRAIN
```

### Initial State

```text
GPIO_LOW
GPIO_HIGH
```

---

# Runtime Configuration

GPIO configuration can also be changed at runtime through the `GPIO_Ioctl()` interface.

Supported commands include:

```text
GPIO_IOCTL_SET_MODE
GPIO_IOCTL_SET_PULL
GPIO_IOCTL_SET_SPEED
GPIO_IOCTL_SET_OUTPUT_TYPE
GPIO_IOCTL_SET_ALTERNATE_FUNC
GPIO_IOCTL_LOCK
```

Example:

```c
GPIO_Mode_t mode = GPIO_MODE_OUTPUT;

GPIO_Ioctl(
    GPIO_LED,
    GPIO_IOCTL_SET_MODE,
    &mode
);
```

Frequently used operations such as read, write, and toggle are intentionally provided as dedicated APIs rather than through `ioctl`.

---

# Basic API

### Initialization

```c
GPIO_Init();
```

Initializes all GPIOs defined in the hardware initialization table.

### Write

```c
GPIO_Write(GPIO_LED, GPIO_HIGH);
```

### Read

```c
GPIO_State_t state;

state = GPIO_Read(GPIO_LED);
```

### Toggle

```c
GPIO_Toggle(GPIO_LED);
```

---

# HAL Layer

The GPIO HAL provides the low-level STM32G4 implementation.

It handles:

* GPIO port clock enable
* `MODER`
* `OTYPER`
* `PUPDR`
* `OSPEEDR`
* `AFR`
* `IDR`
* `ODR`
* `BSRR`
* `LCKR`

The driver layer does not directly manipulate these registers.

This keeps STM32-specific register operations isolated inside the HAL.

---

# Glitch-Free Initialization

`GPIO_HAL_Init()` applies the configured initial output state before changing the GPIO mode to output.

Conceptually:

```text
Configure output parameters
        ↓
Set initial output state
        ↓
Switch GPIO to OUTPUT mode
```

This prevents unwanted output glitches during initialization.

This is particularly useful for sensitive hardware such as motor-control and gate-driver signals.

---

# Automatic GPIO Clock Enable

The HAL automatically enables the required GPIO peripheral clock during initialization.

Therefore, application code does not need to manually enable the GPIO clock before calling:

```c
GPIO_Init();
```

Multiple pins sharing the same GPIO port are also handled safely.

---

# Pin Locking

GPIO pins can be locked through:

```c
GPIO_Ioctl(
    GPIO_LED,
    GPIO_IOCTL_LOCK,
    NULL
);
```

The HAL performs the STM32 GPIO lock sequence to protect the configuration from unintended modification.

---

# Design Philosophy

The GPIO driver follows a simple separation of responsibilities:

```text
Application
    │
    │ Logical GPIO name
    ▼
GPIO Driver
    │
    │ Hardware-independent configuration
    ▼
BSP / Board Mapping
    │
    │ Physical port + pin
    ▼
GPIO HAL
    │
    │ MCU registers
    ▼
STM32G4
```

The important idea is:

> **Application code knows what a pin represents, while the hardware layers know where that pin physically exists.**

---

# Current Status

### Implemented

* Logical GPIO naming
* Hardware-specific GPIO section
* Data-driven initialization
* Board-level physical mapping
* Input mode
* Output mode
* Alternate-function mode
* Analog mode
* Pull-up / pull-down
* Output speed
* Push-pull / open-drain
* Initial output state
* Read / Write / Toggle
* Runtime configuration through `GPIO_Ioctl`
* Alternate-function configuration
* GPIO locking
* Automatic GPIO clock enable
* Glitch-free initialization
* STM32G4 register-level HAL

### Target

```text
STM32G431xx
```

---

# Author

**Md. Shohanur Rahman**

GitHub:
`https://github.com/shohanur00/G4_Core_Lib`

LinkedIn:
`https://www.linkedin.com/in/engr-shohanur-rahman-181a69250/`

---

# License

Copyright All rights reserved.