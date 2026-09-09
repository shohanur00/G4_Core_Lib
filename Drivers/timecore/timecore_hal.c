#include "timecore_hal.h"
#include "stm32g431xx.h"
#include "systemclock/systemclock.h"

#define TIMECORE_TICK_FREQ_HZ    1000UL

static volatile uint32_t timecore_ms = 0U;
volatile uint8_t timecore_update_flag = 0U;


void TimeCore_HAL_Init(void)
{
    uint32_t timer_clock;
    uint32_t prescaler;

    /* Enable TIM2 clock */
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    /* Stop timer */
    TIM2->CR1 = 0U;

    /*
     * TIM2 clock = 170 MHz
     * We need 1 kHz update event = 1 ms
     */
    timer_clock = SystemClock_GetFrequency();

    prescaler = (timer_clock / 1000000UL) - 1UL;

    /*
     * Timer counter = 1 MHz
     * ARR = 999
     * Therefore update event every 1000 us = 1 ms
     */
    TIM2->PSC = prescaler;
    TIM2->ARR = TIMECORE_TICK_FREQ_HZ - 1;

    /* Reset counter */
    TIM2->CNT = 0U;

    /* Generate update event to load PSC */
    TIM2->EGR = TIM_EGR_UG;

    /* Clear update flag */
    TIM2->SR &= ~TIM_SR_UIF;

    /* Enable update interrupt */
    TIM2->DIER |= TIM_DIER_UIE;

    /* Enable TIM2 interrupt in NVIC */
    NVIC_EnableIRQ(TIM2_IRQn);

    /* Start timer */
    TIM2->CR1 |= TIM_CR1_CEN;
}


uint32_t TimeCore_HAL_GetMs(void)
{
    return timecore_ms;
}

void TimeCore_HAL_ResetMs(void)
{
    timecore_ms = 0U;
}

void TimeCore_HAL_SetUpdateFlag(void)
{
    timecore_update_flag = 1U;
}

void TimeCore_HAL_ClearUpdateFlag(void)
{
    timecore_update_flag = 0U;
}

uint8_t TimeCore_HAL_GetUpdateFlag(void)
{
    return timecore_update_flag;
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;

        timecore_ms++;
        TimeCore_HAL_SetUpdateFlag();
    }
}