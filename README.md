<div align="center">

# G4_Core_Lib

**A reusable and modular embedded software foundation for STM32G4 microcontrollers**

Built with Embedded C, CMSIS, and CMake.

![Target MCU](https://img.shields.io/badge/Target%20MCU-STM32G431-blue)
![Language](https://img.shields.io/badge/Language-Embedded%20C-informational)
![Architecture](https://img.shields.io/badge/Architecture-Layered%20%2F%20HAL-orange)
![Build](https://img.shields.io/badge/Build-CMake-lightgrey)
![Status](https://img.shields.io/badge/Status-Actively%20Developing-yellow)

</div>

---

The main goal of **G4_Core_Lib** is to provide a clean and reusable software architecture where application code remains separated from MCU-specific hardware implementation.

| | |
|---|---|
| **Target MCU** | STM32G431 |
| **Language** | Embedded C |
| **Architecture** | Layered / Hardware Abstraction |
| **Development Environment** | VS Code + CMake |
| **HAL** | Custom lightweight HAL using CMSIS |
| **IDE Dependency** | No CubeMX |

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
  - [1. Application Layer](#1-application-layer)
  - [2. Driver Layer](#2-driver-layer)
  - [3. HAL Layer](#3-hal-layer)
  - [4. BSP Layer](#4-bsp-layer)
  - [5. Configuration Layer](#5-configuration-layer)
- [Core Modules](#core-modules)
  - [GPIO](#gpio)
  - [TimeCore](#timecore)
  - [SystemClock](#systemclock)
- [Debug Service](#debug-service)
- [Project Structure](#project-structure)
- [Design Principles](#design-principles)
- [Development Environment](#development-environment)
- [Build](#build)
- [Flash and Debug](#flash-and-debug)
- [Why G4_Core_Lib?](#why-g4_core_lib)
- [Project Status](#project-status)
- [Roadmap](#roadmap)
- [Target Applications](#target-applications)
- [Author](#author)
- [License](#license)

---

## Overview

G4_Core_Lib is designed as a reusable software foundation for STM32G4-based embedded projects.

Instead of configuring peripherals directly inside the application, the library separates the system into multiple layers:

```text
Application
    |
    v
Driver / API Layer
    |
    v
Hardware Abstraction Layer
    |
    v
BSP / Board Layer
    |
    v
STM32G4 Hardware
```

This approach keeps application-level code independent from low-level hardware details and makes individual modules easier to reuse, test, and maintain.

---

## Architecture

The project follows a layered software architecture.

### 1. Application Layer

Located in:

```text
App/
```

The application layer contains the main application logic.

Application code should interact with the system through high-level APIs rather than directly accessing peripheral registers.

### 2. Driver Layer

Located in:

```text
Drivers/
```

The driver layer provides reusable peripheral and system-level components.

Current modules include:

```text
Drivers/
├── gpio/
│   ├── gpio.c
│   ├── gpio.h
│   ├── gpio_hal.c
│   └── gpio_hal.h
│
├── systemclock/
│   ├── systemclock.c
│   └── systemclock.h
│
└── timecore/
    ├── timecore.c
    ├── timecore.h
    ├── timecore_hal.c
    └── timecore_hal.h
```

### 3. HAL Layer

Hardware-specific operations are isolated inside HAL implementations.

For example:

```text
GPIO API
    |
    v
GPIO HAL
    |
    v
STM32G4 Registers
```

The application does not need to know how GPIO registers are configured internally. This keeps the higher-level API clean and makes the driver easier to maintain and extend.

### 4. BSP Layer

Located in:

```text
BSP/
```

The Board Support Package contains board-specific definitions and hardware mappings.

```text
BSP/
├── board.c
└── board.h
```

The BSP layer connects logical resources used by the software to the physical hardware available on the target board.

### 5. Configuration Layer

Located in:

```text
Config/
```

Configuration files centralize project and board-specific settings.

```text
Config/
├── board_config.h
├── debug_config.h
└── project_config.h
```

This prevents configuration values from being scattered throughout the codebase.

---

## Core Modules

### GPIO

The GPIO module provides a reusable interface for GPIO operations while keeping STM32-specific implementation details inside the HAL.

```text
Application
    |
    v
gpio.h / gpio.c
    |
    v
gpio_hal.h / gpio_hal.c
    |
    v
STM32G4 GPIO Registers
```

The design allows application code to work with logical GPIO resources instead of directly manipulating MCU registers.

### TimeCore

`TimeCore` is a lightweight software timing component designed around a hardware-independent timing API. It provides software timer functionality using a periodic system tick.

```text
Application
    |
    v
TimeCore API
    |
    v
TimeCore HAL
    |
    v
Hardware Time Base
```

The intention is to allow application modules to create and manage timers without depending directly on a particular STM32 timer peripheral.

### SystemClock

The `SystemClock` module handles system clock configuration and provides a central interface for clock-related initialization.

```text
Drivers/systemclock/
├── systemclock.c
└── systemclock.h
```

Keeping clock initialization in a dedicated module prevents clock configuration logic from being mixed with application code.

---

## Debug Service

Debug-related functionality is separated under:

```text
Services/Debug/
```

Current structure:

```text
Services/
└── Debug/
    ├── debug.c
    └── debug.h
```

This keeps debugging and diagnostic functionality separate from application logic.

---

## Project Structure

The overall repository is organized as follows:

```text
G4_Core_Lib/
├── App/                    # Application layer
├── BSP/                    # Board Support Package
├── CMSIS/                  # CMSIS core and device files
├── Config/                 # Project and board configuration
├── Drivers/                # Reusable software drivers
│   ├── gpio/
│   ├── systemclock/
│   └── timecore/
├── Services/
│   └── Debug/              # Debug and diagnostic services
├── Src/                    # Startup and system source files
├── cmake/                  # CMake support files
├── .vscode/                # VS Code configuration
├── CMakeLists.txt
├── CMakePresets.json
├── STM32G431.svd
├── stm32g431xb_flash.ld
└── README.md
```

---

## Design Principles

G4_Core_Lib is being developed around several embedded software design principles.

**Hardware Abstraction**
Application code should not depend directly on MCU registers when a reusable abstraction is appropriate.

**Separation of Concerns**
Each layer has a clearly defined responsibility:

```text
Application  -> Application behavior
Driver       -> Public peripheral API
HAL          -> Hardware-specific implementation
BSP          -> Board-specific mapping
CMSIS        -> MCU register definitions
```

**Reusability**
Drivers are designed as independent modules so they can be reused across different embedded projects.

**Encapsulation**
Low-level implementation details remain hidden behind public interfaces.

**Maintainability**
Configuration, board definitions, drivers, and application logic are kept separate to make future modifications easier.

---

## Development Environment

The project is designed around a modern command-line-based embedded development workflow.

**Tools**

- VS Code
- CMake
- ARM GNU Toolchain
- CMSIS
- STM32G431
- Cortex-Debug
- STM32 Programmer / SWD

The project does not depend on STM32CubeMX for peripheral abstraction.

---

## Build

Clone the repository:

```bash
git clone https://github.com/shohanur00/G4_Core_Lib.git
cd G4_Core_Lib
```

Configure the CMake project using the available CMake configuration or presets.

Typical workflow:

```bash
cmake --preset <preset>
cmake --build --preset <preset>
```

The exact preset depends on the local development environment.

---

## Flash and Debug

The project is intended to be programmed through **SWD**.

A typical development setup is:

```text
VS Code
├── CMake
├── ARM GCC
└── Cortex-Debug
        |
        v
    SWD Probe
        |
        v
    STM32G431
```

The repository also includes:

```text
STM32G431.svd
```

which can be used by debugging tools for peripheral and register inspection.

---

## Why G4_Core_Lib?

Many embedded projects start with peripheral configuration directly inside `main.c`. This approach works well for small projects, but can become difficult to maintain as firmware grows.

G4_Core_Lib follows a more modular approach.

<table>
<tr>
<th>Traditional Approach</th>
<th>G4_Core_Lib Approach</th>
</tr>
<tr>
<td>

```text
main.c
├── GPIO configuration
├── Timer configuration
├── Clock configuration
├── Peripheral registers
└── Application logic
```

</td>
<td>

```text
Application
├── GPIO API
├── TimeCore API
└── SystemClock API
        |
        v
     Drivers
        |
        v
       HAL
        |
        v
       BSP
        |
        v
     Hardware
```

</td>
</tr>
</table>

The objective is to make the application layer focus on **what the system should do**, rather than **how STM32 peripheral registers should be configured**.

---

## Project Status

G4_Core_Lib is an actively developing project.

**Completed**

- [x] STM32G4 project foundation
- [x] CMSIS-based development
- [x] CMake build system
- [x] BSP structure
- [x] GPIO driver architecture
- [x] System clock module
- [x] TimeCore software timer module
- [x] Debug service structure

**Planned**

- [ ] Additional reusable peripheral drivers
- [ ] More generic HAL interfaces
- [ ] Extended documentation
- [ ] Unit testing and host-side testing

---

## Roadmap

Planned improvements include:

- UART driver
- SPI driver
- I2C driver
- ADC driver
- DMA abstraction
- PWM driver
- Interrupt management
- Circular buffer utilities
- Additional reusable middleware components
- Improved cross-project portability
- Automated testing
- API documentation

---

## Target Applications

G4_Core_Lib is intended to serve as a reusable foundation for:

- Embedded firmware development
- Motor control applications
- ESC firmware
- Robotics
- Control systems
- Data acquisition
- Industrial embedded systems
- STM32G4-based R&D projects

---

## Author

**Engr. Shohanur Rahman**
Embedded Software & Hardware Engineer

GitHub: [https://github.com/shohanur00](https://github.com/shohanur00)

---

## License

This project is currently under development. License information will be added as the project matures.
