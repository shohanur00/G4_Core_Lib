#include "systemclock.h"
#include "stm32g431xx.h"


#define SYSTEMCLOCK_FREQ_HZ    170000000UL


void SystemClock_Init(void)
{
    // Enable external 8 MHz active oscillator (HSE bypass mode).
    RCC->CR |= RCC_CR_HSEBYP | RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)); // Wait until HSE is ready

    // Configure Flash for 170 MHz before increasing the system clock.
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | (4U << FLASH_ACR_LATENCY_Pos);

    // Configure PLL: 8 MHz / 1 * 85 / 4 = 170 MHz.
   RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE |
               (1U  << RCC_PLLCFGR_PLLM_Pos) |
               (85U << RCC_PLLCFGR_PLLN_Pos) |
               (0U  << RCC_PLLCFGR_PLLR_Pos) |
               RCC_PLLCFGR_PLLREN;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Wait until PLL is ready

    // Select PLL as system clock source.
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Wait until PLL is used as system clock

    // Update SystemCoreClock variable
    // SystemCoreClockUpdate();
    
}


uint32_t SystemClock_GetFrequency(void)
{
    return SYSTEMCLOCK_FREQ_HZ;
}