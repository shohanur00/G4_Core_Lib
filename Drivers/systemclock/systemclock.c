#include "systemclock.h"
#include "stm32g431xx.h"


void SystemClock_Init(void)
{
    /* ---- 1. HSE bypass (external clock, e.g. ST-Link MCO) ---- */
    RCC->CR |= RCC_CR_HSEBYP;      /* bypass MUST be set before HSEON */
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0U) { }

    /* ---- 2. HSI48 (for USB / CRS / RNG) ---- */
    RCC->CRRCR |= RCC_CRRCR_HSI48ON;
    while ((RCC->CRRCR & RCC_CRRCR_HSI48RDY) == 0U) { }

    /* ---- 3. PWR clock + Range1 + Boost mode ---- */
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;

    PWR->CR1 = (PWR->CR1 & ~PWR_CR1_VOS) | (1U << PWR_CR1_VOS_Pos); /* Range 1 */
    while ((PWR->SR2 & PWR_SR2_VOSF) != 0U) { }

    RCC->CFGR |= RCC_CFGR_HPRE_3;      /* temp AHB /2 during boost transition */
    PWR->CR5  &= ~PWR_CR5_R1MODE;      /* enter Range1-boost */
    for (volatile int i = 0; i < 20; i++) { }
    RCC->CFGR &= ~RCC_CFGR_HPRE_3;     /* AHB back to /1 */

    /* ---- 4. Flash latency for 170 MHz ---- */
    FLASH->ACR = FLASH_ACR_LATENCY_4WS | FLASH_ACR_PRFTEN;

    /*
     * ---- 5. PLL configuration ----
     * HSE / 2 = 4 MHz      (PLLM field = 2-1 = 1)
     * 4 MHz x 85 = 340 MHz VCO
     * P = /2 -> 170 MHz (SAI/ADC use)
     * Q = /2 -> 170 MHz (FDCAN/other, NOT used for USB — USB uses HSI48)
     * R = /2 -> 170 MHz (SYSCLK)
     */
    RCC->PLLCFGR =
          RCC_PLLCFGR_PLLSRC_HSE
        | (1U  << RCC_PLLCFGR_PLLM_Pos)     /* DIV2 : field = M-1 = 1 */
        | (85U << RCC_PLLCFGR_PLLN_Pos)     /* N = 85, direct value   */
        | RCC_PLLCFGR_PLLPEN
        | (2U  << RCC_PLLCFGR_PLLPDIV_Pos)  /* PLLP = /2, direct value */
        | RCC_PLLCFGR_PLLQEN
        | (0U  << RCC_PLLCFGR_PLLQ_Pos)     /* DIV2 : field = (Q/2)-1 = 0 */
        | RCC_PLLCFGR_PLLREN
        | (0U  << RCC_PLLCFGR_PLLR_Pos);    /* DIV2 : field = (R/2)-1 = 0 */

    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0U) { }

    /* ---- 6. AHB/APB dividers = /1 ---- */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);

    /* ---- 7. Switch SYSCLK source to PLL ---- */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) { }
    
}


uint32_t SystemClock_GetFrequency(void)
{
    return SYSTEMCLOCK_FREQ_HZ;
}