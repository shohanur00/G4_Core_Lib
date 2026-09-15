<div align="center">

# Logger

![Status](https://img.shields.io/badge/status-active-brightgreen)
![UART](https://img.shields.io/badge/UART-DMA_planned-blue)

</div>

A lightweight and reusable logging service for STM32 embedded applications.

The Logger is designed to provide a simple and hardware-independent logging interface while allowing the output destination to be configured through different backends.

The Logger core does not need to know whether log data is sent through UART, RTT, Flash, USB, RAM, or another output interface.

---

## Features

* Multiple log levels
* Module-based log filtering
* Compile-time log filtering
* Runtime log-level control
* Optional timestamp
* Optional log-level tag
* Optional module name
* Optional ANSI color support
* Multiple output backends
* Backend-independent logging API
* Configurable UART backend
* User customization sections protected by `#pragma USER Starts` / `#pragma USER Ends`

---

## Architecture

The Logger is organized into three main layers:

```text
Application
     │
     ▼
┌───────────────┐
│   Frontend    │
│   logger.h    │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│     Core      │
│   logger.c    │
│ logger_hal.*  │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│   Backends    │
├───────────────┤
│ UART          │
│ RTT           │
│ Flash         │
│ USB           │
│ RAM           │
│ ...           │
└───────────────┘
```

### Frontend

The Frontend provides the public logging API used by the application.

Typical usage:

```c
LOG_INFO(LOG_MODULE_SYSTEM, "System initialized");
LOG_WARNING(LOG_MODULE_SYSTEM, "Temperature is high");
LOG_ERROR(LOG_MODULE_SYSTEM, "Sensor communication failed");
```

The application does not need to know which backend is currently handling the log data.

---

### Core

The Core is responsible for:

* Log-level filtering
* Module-level filtering
* Message formatting
* Timestamp handling
* Log buffer generation
* Forwarding formatted data to the Logger HAL

The Core remains independent of the actual output hardware.

---

### Backends

Backends are responsible for transmitting or storing the formatted log data.

Examples:

```text
UART
RTT
Flash
USB
RAM
Custom
```

Multiple backends can be enabled at the same time.

For example:

```text
LOG_Write()
    │
    ▼
 Logger Core
    │
    ▼
 Logger HAL
    ├── UART
    ├── RTT
    └── Flash
```

---

# Folder Structure

```text
Logger/
│
├── README.md
│
├── Frontend/
│   └── logger.h
│
├── Core/
│   ├── logger.c
│   ├── logger_hal.h
│   └── logger_hal.c
│
└── Backends/
    ├── UART/
    │   ├── log_uart.h
    │   └── log_uart.c
    │
    ├── RTT/
    │   ├── log_rtt.h
    │   └── log_rtt.c
    │
    ├── Flash/
    │   ├── log_flash.h
    │   └── log_flash.c
    │
    ├── USB/
    │   ├── log_usb.h
    │   └── log_usb.c
    │
    └── RAM/
        ├── log_ram.h
        └── log_ram.c
```

Not all backends need to be enabled. Only the required backend implementations need to be included and configured.

---

# Log Levels

The Logger supports the following levels:

```text
DEBUG
INFO
WARNING
ERROR
CRITICAL
```

Example:

```c
LOG_DEBUG(LOG_MODULE_SYSTEM, "ADC value = %u", adc_value);

LOG_INFO(LOG_MODULE_SYSTEM, "System initialized");

LOG_WARNING(LOG_MODULE_SYSTEM, "Battery voltage is low");

LOG_ERROR(LOG_MODULE_SYSTEM, "Sensor communication failed");

LOG_CRITICAL(LOG_MODULE_SYSTEM, "System initialization failed");
```

---

# Module Filtering

Logs can be filtered independently for each module.

Example:

```c
LOG_SetModuleLevel(LOG_MODULE_SYSTEM, LOG_LEVEL_DEBUG);
```

This allows the System module to output DEBUG and higher-level messages.

The filtering mechanism can be used to reduce unnecessary log output while debugging specific parts of an application.

---

# Basic Usage

Initialize the Logger during system initialization:

```c
LOG_Init();
```

Then use the logging macros from the application:

```c
LOG_INFO(LOG_MODULE_SYSTEM, "System started");

LOG_DEBUG(LOG_MODULE_SYSTEM,
          "Temperature = %d C",
          temperature);

LOG_ERROR(LOG_MODULE_SYSTEM,
          "Communication error");
```

---

# Timestamp

The Logger can optionally include a timestamp in each log message.

The timestamp is updated through:

```c
LOG_MainLoop(ref_time);
```

Example:

```c
while (1)
{
    LOG_MainLoop(TimeCore_Get());

    /* Application code */
}
```

The Logger does not directly depend on a specific timer implementation.

This allows the application or time service to provide the reference time.

---

# UART Backend

![Planned](https://img.shields.io/badge/DMA-planned-yellow)

The UART backend is designed as a hardware-specific logging output.

> **Note:** The current UART backend uses blocking/polling-based transmission. A **DMA-based UART transmission mode is planned** for a future release to offload log transmission from the CPU and improve overall system performance, especially under heavy logging load.

UART configuration is defined in the board configuration file:

```text
board.h
```

This keeps board-specific hardware configuration outside the Logger implementation.

Typical configuration includes:

```c
#define LOG_UART_INSTANCE        3U
#define LOG_UART_BAUDRATE        115200U

#define LOG_UART_TX_PORT         GPIOB
#define LOG_UART_TX_PIN          9U
```

The exact configuration depends on the target board.

For example, on an STM32G431 board, USART3 can be selected and its corresponding TX pin can be configured through `board.h`.

The UART backend itself uses this configuration to initialize and operate the selected UART peripheral.

---

# User Customization

Some source files contain dedicated user customization sections:

```c
#pragma USER Starts

/* User code */

#pragma USER Ends
```

Anything placed between these markers is intended for user customization.

### Example

```c
#pragma USER Starts

/* User-defined configuration or code */

#pragma USER Ends
```

Users may add their own code inside these sections without modifying the protected Logger implementation.

### Important

Keep user modifications inside the designated:

```c
#pragma USER Starts
```

and

```c
#pragma USER Ends
```

sections whenever applicable.

This helps keep the original Logger implementation separated from application-specific customization.

---

# Backend Configuration

Backend selection is controlled through the Logger HAL configuration.

Example:

```c
#define LOG_HAL_USE_UART        1U
#define LOG_HAL_USE_FLASH       0U
#define LOG_HAL_USE_USB         0U
#define LOG_HAL_USE_RTT         0U
#define LOG_HAL_USE_RAM         0U
```

Multiple backends can be enabled simultaneously.

For example:

```c
#define LOG_HAL_USE_UART        1U
#define LOG_HAL_USE_RTT         1U
```

In this configuration, the same formatted log message can be forwarded to both UART and RTT.

---

# Design Philosophy

The Logger follows a simple separation-of-responsibility approach:

```text
Application
    │
    │  LOG_INFO()
    ▼
Frontend
    │
    ▼
Core
    │
    │ formatted log data
    ▼
HAL
    │
    ▼
Backend
    │
    ▼
Hardware / Storage / Debug Interface
```

The application should only interact with the Logger API.

It should not need to know:

* Which UART is being used
* Which GPIO pin is connected
* How UART is initialized
* How the log buffer is transmitted
* Where the log data is physically stored

These details belong to the board configuration and backend implementation.

---

# Example Output

Depending on the enabled Logger options, output may look like:

```text
[000123 ms][INFO][SYSTEM] System initialized
[000125 ms][DEBUG][SYSTEM] ADC value = 2048
[000130 ms][WARNING][SYSTEM] Temperature is high
[000135 ms][ERROR][SYSTEM] Sensor communication failed
```

The exact format depends on the Logger configuration.

---

# Adding a New Backend

A new backend can be added without changing the application-level logging API.

For example:

```text
Backends/
└── Custom/
    ├── log_custom.h
    └── log_custom.c
```

The backend should provide an initialization function and a write function following the Logger backend interface.

Example:

```c
void LOG_CUSTOM_Init(void);

void LOG_CUSTOM_Write(const char *buffer, size_t length);
```

The Logger HAL can then forward log messages to the new backend.

---

# Portability

The Logger is designed to keep hardware-specific details isolated.

For a new board:

1. Update the board configuration.
2. Configure the required UART/peripheral.
3. Enable the required backend.
4. Keep the application-level Logger API unchanged.

This allows the same Logger service to be reused across different STM32 projects and boards.

---

# Current Status

### Core

* [x] Log levels
* [x] Module filtering
* [x] Runtime log filtering
* [x] Timestamp support
* [x] Formatted messages
* [x] Logger HAL
* [x] Multiple backend support

### UART Backend

* [x] UART initialization interface
* [x] UART write interface
* [x] Board-based configuration
* [ ] **DMA-based UART transmission (planned)**

### Planned / Extendable Backends

* [ ] RTT
* [ ] Flash
* [ ] USB
* [ ] RAM
* [ ] Custom backend

---

## Author

**Md. Shohanur Rahman**

GitHub:
https://github.com/shohanur00/G4_Core_Lib

LinkedIn:
https://www.linkedin.com/in/engr-shohanur-rahman-181a69250/

---

## License

Copyright © 2026 Md. Shohanur Rahman.

All rights reserved.