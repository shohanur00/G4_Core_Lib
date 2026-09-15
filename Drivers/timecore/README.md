<div align="center">

# TimeCore

![Status](https://img.shields.io/badge/status-active-brightgreen)
![MCU](https://img.shields.io/badge/MCU-STM32G431-blue)
![Timers](https://img.shields.io/badge/software_timers-25-informational)
![Base](https://img.shields.io/badge/time_base-1ms-orange)
![License](https://img.shields.io/badge/license-All%20Rights%20Reserved-lightgrey)

</div>

**TimeCore** is a lightweight and reusable timer management module for embedded systems.

It provides a software timer framework built on top of a configurable hardware timer. The hardware timer generates a **1 ms system time base**, while TimeCore manages multiple software timers using simple and reusable APIs.

The module is designed with a layered approach:

```text
Application
     │
     ▼
 TimeCore
     │
     ▼
TimeCore HAL
     │
     ▼
STM32G4 Hardware Timer
```

The current implementation targets **STM32G431** and uses CMSIS/register-level access without requiring STM32 HAL or CubeMX-generated timer code.

---

## Features

* 1 ms software time base
* Configurable STM32G4 hardware timer
* Up to 25 concurrent software timers
* Timer creation and deletion
* Start / Stop
* Pause / Resume
* Reset
* One-shot expiration events
* Continuous expiration events
* Remaining-time tracking
* Elapsed-time tracking
* Runtime duration configuration
* Safe and forceful timer deletion
* Timer allocation and state checking
* Hardware abstraction through `TimeCore_HAL`
* Interrupt-driven timekeeping
* Automatic timer clock configuration
* Direct CMSIS/register-level implementation
* No CubeMX dependency

---

# Directory Structure

```text
Drivers/
└── timecore/
    ├── timecore.h
    ├── timecore.c
    ├── timecore_hal.h
    └── timecore_hal.c
```

### Responsibilities

| File             | Responsibility                                     |
| ---------------- | -------------------------------------------------- |
| `timecore.h`     | Public TimeCore API                                |
| `timecore.c`     | Software timer management                          |
| `timecore_hal.h` | Hardware abstraction interface and timer selection |
| `timecore_hal.c` | STM32G4 timer implementation                       |

---

# Architecture

TimeCore separates software timer management from the hardware timer implementation.

```text
┌──────────────────────────────┐
│          Application         │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│          TimeCore            │
│                              │
│ Timer creation               │
│ Start / Stop                 │
│ Pause / Resume                │
│ Expiration events             │
│ Remaining / elapsed time      │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│       TimeCore HAL           │
│                              │
│ Timer configuration          │
│ Tick generation               │
│ Update flag                   │
│ Interrupt handling            │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│      STM32G4 TIMx            │
└──────────────────────────────┘
```

The application does not need to directly access timer registers.

---

# Hardware Timer Configuration

The hardware timer is selected inside `timecore_hal.h`.

The module currently provides timer selection macros for:

```text
TIM1
TIM2
TIM3
TIM4
TIM5
TIM6
TIM7
TIM8
TIM15
TIM16
TIM17
TIM20
```

The selected timer is configured through the user customization section.

```c
#pragma USER start

#define TIMECORE_HAL_TIMER    TIMECORE_HAL_TIMER_TIM2

#pragma USER end
```

By default, the current configuration selects:

```c
TIMECORE_HAL_TIMER_TIM2
```

---

# Changing the Hardware Timer

To use another supported timer, modify only the user configuration section:

```c
#pragma USER start

#define TIMECORE_HAL_TIMER    TIMECORE_HAL_TIMER_TIM3

#pragma USER end
```

The HAL automatically maps the selected timer to:

* Timer instance
* IRQ number
* Peripheral clock enable
* Peripheral clock disable
* Interrupt vector

Unsupported timer selections generate a compile-time error.

This keeps the hardware-specific configuration isolated from the TimeCore logic.

---

# Time Base

TimeCore generates a **1 ms software time base**.

The HAL configuration uses:

```text
Timer counter frequency = 1 MHz
Timer counter resolution = 1 µs

Update frequency = 1000 Hz
Update period = 1 ms
```

The timer prescaler is calculated from the system timer clock:

```text
Prescaler =
    Timer Clock / 1,000,000 - 1
```

The auto-reload value is calculated as:

```text
ARR =
    1,000,000 / 1,000 - 1
```

Therefore:

```text
1 timer update event = 1 ms
```

The timer clock is obtained through the project's `SystemClock_GetFrequency()` interface.

---

# Interrupt-Driven Timekeeping

The hardware timer generates an update interrupt every 1 ms.

The interrupt handler:

1. Checks the timer update flag
2. Clears the hardware update flag
3. Increments the system tick
4. Sets the software update flag

Conceptually:

```text
Timer Update Event
        │
        ▼
    Timer IRQ
        │
        ▼
TimeCore_HAL_IRQHandler()
        │
        ├── Clear UIF
        ├── timecore_tick++
        └── updateFlag = 1
```

The public TimeCore layer then consumes this update event inside `TimeCore_MainLoop()`.

---

# System Tick

The current system time can be obtained using:

```c
uint32_t tick = TimeCore_GetTick();
```

The returned value represents elapsed time in milliseconds.

The HAL internally maintains the tick counter:

```c
static volatile uint32_t timecore_tick;
```

and provides:

```c
uint32_t TimeCore_HAL_GetTick(void);
```

The HAL also provides a tick reset function:

```c
void TimeCore_HAL_ResetTick(void);
```

---

# Software Timer Management

TimeCore currently supports up to:

```c
#define TIMECORE_MAX_TIMERS    25U
```

Each timer maintains:

```text
Timer ID
Allocation status
Period
Remaining time
Timer state
```

The timer state machine contains:

```text
INVALID
   │
   ▼
CREATED
   │
   ▼
RUNNING
   │
   ├──────► PAUSED
   │          │
   │          ▼
   │       RUNNING
   │
   ▼
EXPIRED

RUNNING ─────► STOPPED
```

The actual internal timer states are:

```c
TIMECORE_TIMER_INVALID
TIMECORE_TIMER_CREATED
TIMECORE_TIMER_RUNNING
TIMECORE_TIMER_PAUSED
TIMECORE_TIMER_EXPIRED
TIMECORE_TIMER_STOPPED
```

---

# Creating a Timer

Create a timer using:

```c
uint8_t timer_id;

timer_id = TimeCore_CreateTimer(1000U);
```

The argument represents the timer duration in milliseconds.

For example:

```c
timer_id = TimeCore_CreateTimer(1000U);
```

creates a timer with:

```text
Duration = 1000 ms
          = 1 second
```

Timer IDs are **1-based**.

If no timer slot is available:

```c
TimeCore_CreateTimer()
```

returns:

```c
0U
```

---

# Starting a Timer

After creating a timer:

```c
TimeCore_StartTimer(timer_id);
```

Example:

```c
uint8_t timer_id;

timer_id = TimeCore_CreateTimer(1000U);

TimeCore_StartTimer(timer_id);
```

The timer transitions:

```text
CREATED → RUNNING
```

---

# Pause and Resume

A running timer can be paused:

```c
TimeCore_PauseTimer(timer_id);
```

The timer transitions:

```text
RUNNING → PAUSED
```

It can later be resumed:

```c
TimeCore_ResumeTimer(timer_id);
```

Result:

```text
PAUSED → RUNNING
```

The remaining time is preserved while the timer is paused.

---

# Stop and Reset

A timer can be stopped using:

```c
TimeCore_StopTimer(timer_id);
```

To reset a timer:

```c
TimeCore_ResetTimer(timer_id);
```

Reset restores the timer's remaining time to its original period and puts the timer into the `CREATED` state.

All timers can also be reset:

```c
TimeCore_ResetAllTimers();
```

---

# One-Shot Expiration Event

For a one-time event:

```c
if (TimeCore_OneShotExpiredEvent(timer_id))
{
    /* Execute task once */
}
```

The first use can automatically transition a `CREATED` timer into `RUNNING`.

When the timer reaches zero:

```text
RUNNING
   │
   ▼
EXPIRED
```

The function returns `1U` for the expiration event.

After the event, the timer remains expired until it is reset or otherwise reconfigured.

---

# Continuous Expiration Event

For periodic tasks:

```c
if (TimeCore_ContinousExpiredEvent(timer_id))
{
    /* Execute periodic task */
}
```

When the timer expires:

```text
Remaining = 0
     │
     ▼
Reload period
     │
     ▼
RUNNING
```

This allows a single software timer to generate repeated periodic events.

Example:

```c
uint8_t timer_id;

timer_id = TimeCore_CreateTimer(500U);

while (1)
{
    TimeCore_MainLoop();

    if (TimeCore_ContinousExpiredEvent(timer_id))
    {
        /* Executes every 500 ms */
    }
}
```

---

# Checking Timer Status

TimeCore provides several status APIs.

### Check expiration

```c
TimeCore_IsExpired(timer_id);
```

### Check running state

```c
TimeCore_IsRunning(timer_id);
```

### Check paused state

```c
TimeCore_IsPaused(timer_id);
```

### Check allocation

```c
TimeCore_IsAllocated(timer_id);
```

These functions return:

```text
1 → TRUE
0 → FALSE
```

---

# Remaining Time

To obtain the remaining timer duration:

```c
uint32_t remaining;

remaining = TimeCore_GetRemainingTime(timer_id);
```

The result is returned in milliseconds.

For example:

```text
Period    = 1000 ms
Elapsed   = 350 ms
Remaining = 650 ms
```

---

# Elapsed Time

Elapsed time can be obtained using:

```c
uint32_t elapsed;

elapsed = TimeCore_GetElapsedTime(timer_id);
```

The value is returned in milliseconds.

---

# Timer Duration Configuration

TimeCore provides two methods for changing timer duration.

### Secure Duration Update

```c
TimeCore_SetDurationSecurely(
    timer_id,
    2000U
);
```

This API is intended for controlled duration changes and only operates for timers in the allowed states defined by the implementation.

### Forceful Duration Update

```c
TimeCore_SetDurationForcefully(
    timer_id,
    2000U
);
```

This updates the timer duration whenever the timer is allocated.

Both functions update:

```text
period_ms
remaining_ms
```

---

# Timer Deletion

Two deletion methods are available.

### Safe Delete

```c
TimeCore_SafeDeleteTimer(timer_id);
```

Safe deletion is intended for an expired timer.

### Force Delete

```c
TimeCore_ForceDeleteTimer(timer_id);
```

Force deletion removes an allocated timer regardless of its current state.

To remove all timers:

```c
TimeCore_DeleteAllTimers();
```

---

# Main Processing

TimeCore requires periodic execution of:

```c
TimeCore_MainLoop();
```

A typical application loop:

```c
while (1)
{
    TimeCore_MainLoop();

    /* Application tasks */
}
```

`TimeCore_MainLoop()` checks the HAL update flag.

When a 1 ms update is detected, it clears the flag and decrements the remaining time of all timers currently in the `RUNNING` state.

---

# Initialization

Initialize TimeCore using:

```c
TimeCore_Init();
```

This performs:

```text
Initialize software timer structures
        ↓
Initialize TimeCore HAL
        ↓
Configure selected hardware timer
        ↓
Configure timer interrupt
        ↓
Start hardware timer
```

---

# Deinitialization

The module can be deinitialized using:

```c
TimeCore_Deinit();
```

This resets the software timer structures and deinitializes the hardware timer HAL.

The hardware timer is stopped, its interrupt is disabled, and its peripheral clock is disabled.

---

# Complete Example

```c
#include "timecore.h"

int main(void)
{
    uint8_t timer_id;

    TimeCore_Init();

    timer_id = TimeCore_CreateTimer(1000U);

    TimeCore_StartTimer(timer_id);

    while (1)
    {
        TimeCore_MainLoop();

        if (TimeCore_OneShotExpiredEvent(timer_id))
        {
            /* Execute task after 1 second */
        }
    }
}
```

---

# Periodic Task Example

```c
#include "timecore.h"

int main(void)
{
    uint8_t timer_id;

    TimeCore_Init();

    timer_id = TimeCore_CreateTimer(500U);

    TimeCore_StartTimer(timer_id);

    while (1)
    {
        TimeCore_MainLoop();

        if (TimeCore_ContinousExpiredEvent(timer_id))
        {
            /* Runs every 500 ms */
        }
    }
}
```

---

# Multiple Timers

Multiple software timers can run independently.

```c
uint8_t led_timer;
uint8_t sensor_timer;
uint8_t communication_timer;

led_timer = TimeCore_CreateTimer(500U);
sensor_timer = TimeCore_CreateTimer(100U);
communication_timer = TimeCore_CreateTimer(1000U);

TimeCore_StartTimer(led_timer);
TimeCore_StartTimer(sensor_timer);
TimeCore_StartTimer(communication_timer);
```

The timers are managed from the same 1 ms hardware time base.

This allows multiple periodic or one-shot tasks without allocating a separate hardware timer for every task.

---

# HAL Design

The TimeCore HAL isolates STM32-specific timer operations from the software timer manager.

The HAL provides:

```c
void TimeCore_HAL_Init(void);

void TimeCore_HAL_Deinit(void);

uint32_t TimeCore_HAL_GetTick(void);

void TimeCore_HAL_ResetTick(void);

void TimeCore_HAL_ClearUpdateFlag(void);

uint8_t TimeCore_HAL_GetUpdateFlag(void);

void TimeCore_HAL_IRQHandler(void);
```

This separation allows the TimeCore logic to remain independent from the low-level timer register implementation.

---

# Hardware Register Access

The current HAL uses CMSIS device definitions and directly configures the STM32G4 timer registers.

The implementation configures:

```text
PSC
ARR
CNT
EGR
SR
DIER
CR1
```

and uses NVIC for the timer interrupt.

No STM32 HAL timer driver is required by TimeCore.

---

# Hardware Customization

The hardware timer selection is intentionally isolated in the `#pragma USER` section:

```c
#pragma USER start

#define TIMECORE_HAL_TIMER    TIMECORE_HAL_TIMER_TIM2

#pragma USER end
```

This section is the primary user customization point for selecting the hardware timer.

The remaining timer-specific register mappings are handled internally by the HAL.

---

# Design Philosophy

TimeCore follows a simple principle:

> **Use one hardware timer to create a reliable system time base, then manage multiple software timers on top of it.**

Instead of dedicating a hardware timer to every application task:

```text
Hardware Timer
      │
      ▼
   1 ms Tick
      │
      ├── Timer 1 → LED
      ├── Timer 2 → Sensor
      ├── Timer 3 → Communication
      ├── Timer 4 → Control Task
      └── Timer N → Application Task
```

This keeps the application timer logic lightweight and reusable.

---

# Current Status

### Implemented

* 1 ms time base
* Hardware timer selection
* Timer clock configuration
* Timer interrupt handling
* System tick
* Update event flag
* Multiple software timers
* 25 timer slots
* Timer lifecycle management
* Start / Stop
* Pause / Resume
* Reset
* One-shot expiration
* Continuous expiration
* Remaining time
* Elapsed time
* Safe deletion
* Force deletion
* Duration update
* HAL abstraction
* CMSIS/register-level implementation
* User-configurable hardware timer

### Current Target

```text
STM32G431
```

### Current Default Timer

```text
TIM2
```

---

# Notes

* Timer durations are specified in milliseconds.
* The software timer resolution is currently 1 ms.
* The hardware timer counter itself is configured for 1 MHz, providing a 1 µs counter resolution.
* `TimeCore_MainLoop()` must be executed regularly by the application.
* Timer IDs are 1-based.
* A return value of `0U` from `TimeCore_CreateTimer()` indicates that no timer slot is available.
* The current implementation provides 25 software timer slots.

---

# Author

**Md. Shohanur Rahman**

GitHub:
`https://github.com/shohanur00/G4_Core_Lib/tree/main/Drivers/timecore`

LinkedIn:
`https://www.linkedin.com/in/engr-shohanur-rahman-181a69250/`

---

# License

Copyright All rights reserved.