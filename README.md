<div align="center">

# G4_Core_Lib

### A Modular Bare-Metal Firmware Foundation for STM32G4

**Embedded C · CMSIS · CMake · ARM GNU Toolchain · VS Code**

<br/>

![Target MCU](https://img.shields.io/badge/MCU-STM32G431CBT6-03234B?logo=stmicroelectronics&logoColor=white)
![Core](https://img.shields.io/badge/Core-ARM%20Cortex--M4-0091BD?logo=arm&logoColor=white)
![Language](https://img.shields.io/badge/Language-Embedded%20C-A8B9CC?logo=c&logoColor=black)
![Architecture](https://img.shields.io/badge/Architecture-Layered%20%2F%20HAL-orange)
![Framework](https://img.shields.io/badge/Framework-CMSIS%20%7C%20Bare--Metal-informational)
![CubeMX](https://img.shields.io/badge/CubeMX-Not%20Required-success)

![Build](https://img.shields.io/badge/Build-CMake%20%2B%20Ninja-064F8C?logo=cmake&logoColor=white)
![Toolchain](https://img.shields.io/badge/Toolchain-ARM%20GNU-red)
![IDE](https://img.shields.io/badge/IDE-VS%20Code-007ACC?logo=visualstudiocode&logoColor=white)
![Debug](https://img.shields.io/badge/Debug-SWD%20%2F%20Cortex--Debug-blueviolet)

![Bootloader](https://img.shields.io/badge/Bootloader-UART%20Update-success)
![Integrity](https://img.shields.io/badge/Integrity-CRC--16--CCITT--FALSE-blueviolet)
![Host Tool](https://img.shields.io/badge/Host%20Tool-Python-3776AB?logo=python&logoColor=white)
![USB](https://img.shields.io/badge/USB-Planned-orange)
![CAN](https://img.shields.io/badge/CAN-Planned-orange)
![OTA](https://img.shields.io/badge/OTA-Planned-orange)

![Status](https://img.shields.io/badge/Status-Actively%20Developing-yellow)
![Last Commit](https://img.shields.io/github/last-commit/shohanur00/G4_Core_Lib)
![Repo Size](https://img.shields.io/github/repo-size/shohanur00/G4_Core_Lib)
![Stars](https://img.shields.io/github/stars/shohanur00/G4_Core_Lib?style=flat)

<br/>

*Building reusable embedded firmware from the hardware layer up.*

</div>

---

## Table of Contents

- [Overview](#overview)
- [Highlights](#highlights)
- [Project Goals](#project-goals)
- [Architecture](#architecture)
- [Core Components](#core-components)
- [Bootloader](#bootloader)
- [Memory Layout](#memory-layout)
- [Host Tools](#host-tools)
- [Build System](#build-system)
- [Quick Start](#quick-start)
- [Project Structure](#project-structure)
- [Design Principles](#design-principles)
- [Development Environment](#development-environment)
- [Flash and Debug](#flash-and-debug)
- [Current Status](#current-status)
- [Roadmap](#roadmap)
- [Target Applications](#target-applications)
- [Author](#author)
- [License](#license)

---

## Overview

**G4_Core_Lib** is a reusable, modular, hardware-oriented firmware foundation for **STM32G4 microcontrollers**, built from the ground up with:

| | |
|---|---|
| **Language** | Embedded C |
| **Core / registers** | CMSIS |
| **Abstraction** | Custom lightweight HAL |
| **Build** | CMake + Ninja |
| **Toolchain** | ARM GNU Toolchain |
| **IDE / debug** | VS Code, SWD, Cortex-Debug |

The project does **not** depend on STM32CubeMX-generated application code. Its main objective is to keep firmware cleanly layered so it is easy to reuse, test, debug, extend, and adapt to future STM32 projects.

```mermaid
flowchart TD
    A["Application Logic"] --> B["Reusable Drivers"]
    B --> C["Hardware Abstraction (HAL)"]
    C --> D["Board Support (BSP)"]
    D --> E["STM32 Hardware"]
```

## Highlights

- 🧱 **Layered architecture**: Application → Driver API → HAL → BSP → CMSIS
- 🔌 **Reusable UART framework** with multi-instance support, interrupt reception, and ring buffers
- ⏱️ **TimeCore** software timing framework, independent of the timer peripheral
- 🪵 **Logger / debug service** decoupled from the UART implementation
- 💾 **Flash abstraction**: erase, program, read, verify, and CRC
- 🚀 **Modular STM32 bootloader** with metadata, CRC verification, retry handling, and application jump
- 🐍 **Python host updater** for firmware update testing
- 🔧 **One CMake project** that builds both Bootloader and Main application configurations

## Project Goals

G4_Core_Lib is being developed as more than a collection of peripheral drivers. The long-term goal is a reusable embedded software foundation covering:

| Area | Scope |
|------|-------|
| Abstraction | Hardware abstraction, peripheral drivers, board support |
| Services | System services, debug and logging |
| Storage | Flash management |
| Updates | Firmware update infrastructure, bootloader, application validation |
| Build | Modular build configurations |

It also serves as a development platform for experimenting with embedded firmware architecture, low-level STM32 development, and reusable software components.

---

## Architecture

```mermaid
flowchart TD
    APP["<b>Application</b><br/>Application-level logic"]
    DRV["<b>Driver / API</b><br/>GPIO · UART · TimeCore · Flash"]
    HAL["<b>HAL</b><br/>Hardware-specific handling"]
    BSP["<b>BSP</b><br/>Board pins · resources"]
    HW["<b>STM32G4 / CMSIS</b>"]
    SVC["<b>Services</b><br/>Debug / Logger"]

    APP --> DRV
    APP --> SVC
    SVC --> DRV
    DRV --> HAL
    HAL --> BSP
    BSP --> HW
```

| Layer | Responsibility |
|-------|----------------|
| **Application** | System-level behavior. Avoids direct dependency on peripheral registers whenever a reusable API exists |
| **Driver / API** | Reusable interfaces for peripherals and system services (`GPIO`, `UART`, `TimeCore`, `Flash`, `SystemClock`) |
| **HAL** | Isolates MCU-specific implementation details |
| **BSP** | Board-specific resources and physical pin mappings (`LED`, `LOG_UART`, `BOOTLOADER_UART`) |
| **Services** | Cross-cutting functionality such as debug and logging |
| **CMSIS** | MCU core and register definitions |

---

## Core Components

### GPIO

Reusable GPIO interface with hardware-specific implementation isolated through the HAL. Applications work with logical GPIO resources without touching MCU registers.

```mermaid
flowchart LR
    A["Application"] --> B["GPIO API"] --> C["GPIO HAL"] --> D["STM32 GPIO Registers"]
```

### TimeCore

A lightweight software timing framework built around a hardware-independent API, so application modules can use software timers without depending on a specific timer peripheral. The current implementation is based on a periodic system time base.

```mermaid
flowchart LR
    A["Application"] --> B["TimeCore API"] --> C["TimeCore HAL"] --> D["Hardware Time Base"]
```

### SystemClock

Centralized system clock initialization, kept separate so clock setup never mixes with application logic.

```text
Drivers/
└── systemclock/
    ├── systemclock.c
    └── systemclock.h
```

### UART Framework

A reusable UART framework designed around multiple instances:

`USART1` · `USART2` · `USART3` · `UART4` · `UART5` · `LPUART1`

```mermaid
flowchart LR
    A["UART API"] --> B["UART HAL"] --> C["STM32 USART / UART Peripheral"]
    C -. "RX interrupt" .-> R["Ring Buffer"]
    R -.-> A
```

It supports interrupt-driven reception and ring-buffer based data handling, forming the foundation for debug logging, bootloader communication, command interfaces, and firmware update transport.

### Logger / Debug Service

Debug and logging live in `Services/Debug/`, separated from the application layer. The logger produces structured diagnostic messages without forcing application code to depend on UART details, and logging can be enabled or disabled through configuration instead of scattered conditional compilation.

```mermaid
flowchart LR
    A["Application"] --> B["Logger"] --> C["Debug Transport"] --> D["UART"]
```

### Flash Framework

A reusable abstraction for STM32 internal Flash, also used by the bootloader firmware update system. It isolates:

- Address validation
- Flash erase and programming
- Data verification and memory access
- CRC calculation
- Flash controller handling

```mermaid
flowchart LR
    A["Application / Bootloader"] --> B["Flash API"] --> C["Flash HAL"] --> D["STM32 Flash Controller"]
```

---

## Bootloader

One of the major developments inside G4_Core_Lib is a reusable **STM32 firmware bootloader**, designed as a modular system rather than update logic buried in `main.c`.

> 📘 Full details: [`Bootloader/README.md`](Bootloader/README.md)

```mermaid
flowchart TD
    BL["Bootloader"]
    BL --> APPL["Bootloader Application"]
    BL --> PROTO["Protocol"]
    BL --> TRANS["Transport"]
    BL --> FLASH["Flash"]
    BL --> META["Metadata"]
    BL --> VALID["Application Validation"]
    BL --> JUMP["Jump Handling"]
    BL --> CFG["Configuration"]
```

### Firmware update architecture

The update logic is independent of the physical transport, so the same protocol can be reused over different links.

```mermaid
flowchart TD
    H["Host / Updater (Python)"] -->|"Transport"| B["Bootloader<br/>Protocol · Update Engine"]
    B --> F["Flash Layer"]
    F --> M["STM32 Internal Flash"]
```

| Transport | Status |
|-----------|--------|
| UART | ✅ Working |
| USB | 📋 Planned |
| CAN | 📋 Planned |
| OTA (wireless) | 📋 Planned |

### Bootloader protocol

The bootloader uses a custom framed protocol with these characteristics:

| Property | Value |
|----------|-------|
| Start of frame (`SOF`) | `0xA5` |
| Maximum payload | 16 bytes |
| CRC | CRC-16-CCITT-FALSE |

Supported commands cover synchronization, device identification, firmware update request, firmware size, application start address, firmware data transfer, ACK, NACK, retransmission, update completion, and application validation. The protocol layer does not depend on UART, which keeps it reusable with other transports. See the [Bootloader README](Bootloader/README.md) for the exact packet layout and command table.

### Update state machine

```mermaid
stateDiagram-v2
    [*] --> WAIT_SYNC
    WAIT_SYNC --> CONNECTED: SYNC
    CONNECTED --> WAIT_FW_LENGTH: Device ID
    WAIT_FW_LENGTH --> WAIT_FW_START_ADDRESS: Firmware size
    WAIT_FW_START_ADDRESS --> READY: Start address
    READY --> RECEIVING: Update start
    RECEIVING --> PROGRAMMING: Data packets
    PROGRAMMING --> VALIDATION: Image complete
    VALIDATION --> COMPLETE: CRC OK
    COMPLETE --> [*]
```

### Update sequence

```mermaid
sequenceDiagram
    participant H as PC / Host
    participant B as Bootloader
    participant F as Flash
    participant M as Metadata

    H->>B: SYNC
    B-->>H: Device ID
    H->>B: Firmware size
    H->>B: Application address
    loop Firmware data packets
        H->>B: Data
        B->>F: Program
        F-->>B: Read-back verify
        B-->>H: ACK / NACK / RETX
    end
    B->>B: CRC validation
    B->>M: Metadata commit
    B-->>H: Update complete
    Note over B,M: Application is VALID
```

### Application validation

The bootloader does not rely on the mere existence of firmware in Flash. It checks firmware state and integrity before allowing execution, which is stronger than testing for non-`0xFF` data.

```mermaid
flowchart TD
    A["Application Address"] --> B["Flash Range"]
    B --> C["Application Metadata"]
    C --> D["Vector Table"]
    D --> E["Initial MSP"]
    E --> F["Reset Handler"]
    F --> G["Firmware CRC"]
    G --> H(["Application VALID"])
```

### Firmware metadata

Persistent metadata, stored separately from the application image, decides whether an installed firmware is valid. It holds:

`Magic` · `Start Address` · `Firmware Size` · `CRC` · `Version` · `Update Status` · `Reserved`

The bootloader trusts persistent metadata, not temporary runtime state.

### Application jump

After successful validation the bootloader hands execution to the application and handles the required Cortex-M state transition:

```mermaid
flowchart TD
    A["Disable bootloader interrupts"] --> B["Clear pending interrupts"]
    B --> C["Stop bootloader system tick"]
    C --> D["Deinitialize bootloader peripherals"]
    D --> E["Relocate vector table"]
    E --> F["Load application MSP"]
    F --> G(["Jump to Reset Handler"])
```

This keeps the application independent from the bootloader runtime environment.

---

## Memory Layout

For the current STM32G431 bootloader configuration, the 128 KB Flash is split into dedicated regions.

| Region | Start | End | Size |
|--------|-------|-----|------|
| Bootloader | `0x08000000` | `0x08003FFF` | 16 KB |
| Main Application | `0x08004000` | `0x0801EFFF` | 108 KB |
| Bootloader Metadata | `0x0801F000` | `0x0801F7FF` | 2 KB |
| Application Data / Reserved | `0x0801F800` | `0x0801FFFF` | 2 KB |

```text
STM32G431CBT6 · 128 KB Flash

0x08000000 ┌──────────────────────────────┐
           │         Bootloader           │
           │            16 KB             │
0x08004000 ├──────────────────────────────┤
           │       Main Application       │
           │            108 KB            │
0x0801F000 ├──────────────────────────────┤
           │     Bootloader Metadata      │
           │             2 KB             │
0x0801F800 ├──────────────────────────────┤
           │   Application Data /         │
           │   Reserved Region   2 KB     │
0x08020000 └──────────────────────────────┘
```

The exact memory configuration is controlled by the linker script (`stm32g431xb_flash.ld`) and the bootloader configuration.

---

## Host Tools

| Tool | Description | Status |
|------|-------------|--------|
| `firmware_update_test.py` | Command-line Python tool used to test the bootloader update flow | ✅ Working |
| [Universal Firmware Updater](https://github.com/shohanur00/Universal_Firmware_Updater) | A user-friendly PC application evolving from the command-line workflow | 🔄 In development |

```mermaid
flowchart LR
    CLI["firmware_update_test.py<br/>(current)"] --> UFU["Universal Firmware Updater<br/>(PC application)"]
    UFU --> UART["UART (current)"]
    UFU --> USB["USB (planned)"]
    UFU --> CAN["CAN (planned)"]
    UFU --> OTA["OTA (planned)"]
```

---

## Build System

The project uses **CMake + Ninja** with the ARM GNU Toolchain, organized as a single VS Code / CMake project where different firmware configurations are built from the same codebase.

| Preset | Purpose |
|--------|---------|
| `Debug` / `Release` | General builds |
| `Debug-Bootloader` / `Release-Bootloader` | Bootloader image, linked for the bootloader Flash region |
| `Debug-Main` / `Release-Main` | Main application image, linked for the application Flash region |

```mermaid
flowchart LR
    SRC["Same codebase"] --> P1["Bootloader preset<br/>0x08000000 · 16 KB"]
    SRC --> P2["Main preset<br/>0x08004000 · 108 KB"]
    P1 --> O1["bootloader image"]
    P2 --> O2["application image"]
```

## Quick Start

**Prerequisites:** VS Code, ARM GNU Toolchain, CMake, Ninja, an ST-LINK (SWD) probe, and Python for the host updater.

```bash
git clone https://github.com/shohanur00/G4_Core_Lib.git
cd G4_Core_Lib

cmake --preset <preset>
cmake --build --preset <preset>
```

Use the exact preset names listed in `CMakePresets.json` (for example `Debug-Bootloader` or `Debug-Main`).

---

## Project Structure

```text
G4_Core_Lib/
│
├── App/                  # Application logic
├── BSP/                  # Board-specific resources
├── CMSIS/                # ARM / STM32 CMSIS files
├── Config/               # Project and board configuration
│
├── Drivers/
│   ├── GPIO
│   ├── UART
│   ├── Flash
│   ├── SystemClock
│   └── TimeCore
│
├── Services/
│   └── Debug/            # Debug / Logger services
│
├── Bootloader/
│   ├── Bootloader core
│   ├── Protocol
│   ├── Transport
│   ├── Flash
│   ├── Metadata
│   └── Application validation
│
├── Src/                  # Startup / system sources
├── Version/              # Project version information
├── cmake/                # CMake support files
├── .vscode/              # VS Code configuration
│
├── CMakeLists.txt
├── CMakePresets.json
├── STM32G431.svd
├── stm32g431xb_flash.ld
└── README.md
```

> The repository structure may evolve as additional reusable modules are introduced.

---

## Design Principles

| Principle | What it means |
|-----------|---------------|
| **Hardware abstraction** | Application code does not depend on MCU registers when a reusable abstraction fits |
| **Separation of concerns** | Application = behavior · Driver = reusable API · HAL = hardware implementation · BSP = board mapping · Services = cross-cutting functions · CMSIS = core and registers |
| **Reusability** | Modules are reusable across projects instead of tied to one application |
| **Encapsulation** | Register-level details stay behind well-defined interfaces (for example Application → Flash API → Flash implementation → Flash controller) |
| **Configuration driven** | Board and project configuration is centralized instead of scattering `#define` values and pin mappings through application code |

---

## Development Environment

| Tool | Purpose |
|------|---------|
| VS Code | Development environment |
| ARM GNU Toolchain | C/C++ compiler and linker |
| CMake | Build configuration |
| Ninja | Build execution |
| CMSIS | MCU / core register definitions |
| Cortex-Debug | Debugging |
| ST-LINK / SWD | Programming and debugging |
| STM32CubeProgrammer | Flash / programming utility |
| Python | Host-side firmware updater |

STM32CubeMX is **not required** for the project's peripheral abstraction architecture.

## Flash and Debug

The project is developed and debugged primarily through **SWD**.

```mermaid
flowchart LR
    VS["VS Code"] --> CM["CMake"]
    VS --> GCC["ARM GCC"]
    VS --> CD["Cortex-Debug"]
    CD --> ST["ST-LINK"]
    ST -->|"SWDIO / SWCLK"| MCU["STM32G431"]
```

The repository includes `STM32G431.svd`, which lets debugging tools show MCU peripheral and register-level inspection.

### Development philosophy

G4_Core_Lib is intentionally developed without a generated framework defining the whole architecture. The goal is to understand and control the underlying system from the bottom up:

```mermaid
flowchart LR
    A["Clock"] --> B["GPIO"] --> C["Interrupt"] --> D["UART"] --> E["Timer"] --> F["Flash"] --> G["Bootloader"] --> H["Application"]
```

This makes the project suitable both as a reusable library and as a foundation for deeper embedded firmware development.

---

## Current Status

G4_Core_Lib is an **actively developing project**.

### Established

| Area | Item | Status |
|------|------|--------|
| Foundation | STM32G431 project foundation, CMSIS-based development | ✅ |
| Build | CMake build system, Ninja workflow, VS Code workflow | ✅ |
| Build | Separate Bootloader / Main configurations, application linker and memory separation | ✅ |
| Structure | BSP structure, configuration layer | ✅ |
| Drivers | GPIO abstraction, system clock module, TimeCore software timing | ✅ |
| UART | UART framework, interrupt-driven reception, ring-buffer reception | ✅ |
| Services | Logger / debug service | ✅ |
| Flash | Flash abstraction, erase / program / verify, CRC-based integrity checking | ✅ |
| Bootloader | Architecture, firmware update protocol, metadata handling | ✅ |
| Bootloader | Application validation, bootloader-to-application jump | ✅ |
| Host | Python command-line updater (`firmware_update_test.py`) | ✅ |

### In development

- [ ] Additional reusable peripheral drivers
- [ ] Broader HAL portability
- [ ] USB support
- [ ] Additional firmware update transports (USB, CAN, OTA)
- [ ] More extensive application validation
- [ ] Power-loss / interrupted-update testing
- [ ] Firmware authentication / secure update mechanisms
- [ ] Universal Firmware Updater PC application
- [ ] Automated and host-side testing
- [ ] API documentation

---

## Roadmap

```mermaid
flowchart TD
    NOW["<b>Now</b><br/>Core drivers · UART bootloader · Python updater"]
    NOW --> P["<b>Peripheral Drivers</b><br/>SPI · I2C · ADC · DMA · PWM · Timers · Interrupt mgmt"]
    NOW --> M["<b>Middleware</b><br/>Circular buffers · Protocol and CRC utilities · NV storage helpers"]
    NOW --> F["<b>Firmware Infrastructure</b><br/>USB · CAN · OTA · Firmware packages · Secure update"]
    NOW --> E["<b>Engineering</b><br/>Unit tests · Static analysis · CI · API docs"]
```

<details>
<summary><b>Detailed roadmap</b></summary>

**Peripheral drivers**
- UART improvements, SPI, I2C, ADC, DMA, PWM
- Additional timer functionality
- Interrupt management

**Middleware / utilities**
- Circular buffers and data structures
- Protocol utilities and CRC utilities
- Non-volatile storage helpers
- Additional reusable system services

**Firmware infrastructure**
- USB communication and USB-based firmware update
- CAN-based firmware update
- OTA update infrastructure
- Firmware package handling
- Improved application validation
- Firmware authentication and secure firmware update architecture

**Engineering infrastructure**
- Unit testing and host-side testing
- Automated build validation
- Static analysis
- API documentation
- Improved cross-project portability

</details>

### Future direction

The project is evolving from a peripheral abstraction library into a broader **embedded firmware platform**.

```mermaid
flowchart TD
    CORE["<b>G4_Core_Lib</b>"]
    CORE --> APP["Application"]
    CORE --> DRV["Drivers"]
    CORE --> SVC["Services"]
    APP --> HAL["HAL"]
    DRV --> HAL
    SVC --> HAL
    HAL --> BSP["BSP"]
    BSP --> HW["STM32G4 Hardware"]
    HW --> MAIN["Application"]
    HW --> BL["Bootloader"]
    BL --> PROTO["Protocol"]
    BL --> FL["Flash"]
    BL --> META["Metadata"]
    PROTO --> FU["Firmware Update"]
```

The long-term goal is to make the same core architecture usable across multiple STM32 embedded projects while keeping hardware-specific implementation isolated and replaceable.

---

## Target Applications

| Domain | Examples |
|--------|----------|
| Motor control | BLDC / FOC development, ESC firmware |
| Robotics and industrial | Robotics, industrial control |
| Measurement | Data acquisition, sensor interfaces, embedded test equipment |
| Power | Power electronics |
| Firmware | Embedded firmware, firmware update systems |
| R&D | STM32-based R&D platforms |

---

## Author

**Engr. Shohanur Rahman**
Embedded Software & Hardware Engineer
Firmware · PCB Hardware · Motor Control · Power Electronics

[![GitHub](https://img.shields.io/badge/GitHub-shohanur00-181717?logo=github&logoColor=white)](https://github.com/shohanur00)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Engr.%20Shohanur%20Rahman-0A66C2?logo=linkedin&logoColor=white)](https://www.linkedin.com/in/engr-shohanur-rahman-181a69250/)

## License

This project is currently under active development. License information will be added as the project matures.

---

<div align="center">

**G4_Core_Lib**

*Building reusable embedded firmware from the hardware layer up.*

</div>
