/**
 * @file timecore_hal.h
 * @brief Hardware Abstraction Layer for TimeCore module
 * @author [Md. Shohanur Rahman]
 * @version 1.0.0
 * @date [09/09/2026]
 * 
 * @details
 * Provides hardware abstraction layer functions for time core management,
 * including initialization, millisecond tracking, and update flag handling.
 */

#ifndef TIMECORE_HAL_H
#define TIMECORE_HAL_H

#include <stdint.h>
#include "stm32g431xx.h"


/* ============================================================
 * TimeCore Timer Selection
 * ============================================================ */

#define TIMECORE_HAL_TIMER_TIM1       1U
#define TIMECORE_HAL_TIMER_TIM2       2U
#define TIMECORE_HAL_TIMER_TIM3       3U
#define TIMECORE_HAL_TIMER_TIM4       4U
#define TIMECORE_HAL_TIMER_TIM5       5U
#define TIMECORE_HAL_TIMER_TIM6       6U
#define TIMECORE_HAL_TIMER_TIM7       7U
#define TIMECORE_HAL_TIMER_TIM8       8U
#define TIMECORE_HAL_TIMER_TIM15     15U
#define TIMECORE_HAL_TIMER_TIM16     16U
#define TIMECORE_HAL_TIMER_TIM17     17U
#define TIMECORE_HAL_TIMER_TIM20     20U


/* ============================================================
 * Selected Timer
 * ============================================================ */

#pragma HARDWARE start

#define TIMECORE_HAL_TIMER            TIMECORE_HAL_TIMER_TIM2

#pragma HARDWARE end
/* ============================================================
 * Timer Hardware Mapping
 * ============================================================ */

#if (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM1)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM1
    #define TIMECORE_HAL_TIMER_IRQn        TIM1_UP_TIM16_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB2ENR |= RCC_APB2ENR_TIM1EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM2)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM2
    #define TIMECORE_HAL_TIMER_IRQn        TIM2_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM3)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM3
    #define TIMECORE_HAL_TIMER_IRQn        TIM3_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM4)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM4
    #define TIMECORE_HAL_TIMER_IRQn        TIM4_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM5)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM5
    #define TIMECORE_HAL_TIMER_IRQn        TIM5_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM6)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM6
    #define TIMECORE_HAL_TIMER_IRQn        TIM6_DAC_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM7)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM7
    #define TIMECORE_HAL_TIMER_IRQn        TIM7_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM8)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM8
    #define TIMECORE_HAL_TIMER_IRQn        TIM8_UP_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB2ENR |= RCC_APB2ENR_TIM8EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM15)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM15
    #define TIMECORE_HAL_TIMER_IRQn        TIM15_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB2ENR |= RCC_APB2ENR_TIM15EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM16)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM16
    #define TIMECORE_HAL_TIMER_IRQn        TIM1_UP_TIM16_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB2ENR |= RCC_APB2ENR_TIM16EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM17)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM17
    #define TIMECORE_HAL_TIMER_IRQn        TIM1_TRG_COM_TIM17_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB2ENR |= RCC_APB2ENR_TIM17EN)


#elif (TIMECORE_HAL_TIMER == TIMECORE_HAL_TIMER_TIM20)

    #define TIMECORE_HAL_TIMER_INSTANCE    TIM20
    #define TIMECORE_HAL_TIMER_IRQn        TIM20_UP_IRQn
    #define TIMECORE_HAL_TIMER_ENABLE()    \
            (RCC->APB2ENR |= RCC_APB2ENR_TIM20EN)


#else

    #error "Unsupported timer selected for TimeCore HAL"

#endif


/* ============================================================
 * HAL Interface
 * ============================================================ */

void TimeCore_HAL_Init(void);

uint32_t TimeCore_HAL_GetMs(void);

void TimeCore_HAL_ResetMs(void);

void TimeCore_HAL_ClearUpdateFlag(void);

uint8_t TimeCore_HAL_GetUpdateFlag(void);

void TimeCore_HAL_IRQHandler(void);


#endif /* TIMECORE_HAL_H */