#include "bl_jump.h"

#include "stm32g431xx.h"
#include "../flash/bl_flash_config.h"

/* --------------------------------------------------------------------------
 * Private Definitions
 * -------------------------------------------------------------------------- */

#define BL_APP_STACK_POINTER_OFFSET    0x00U
#define BL_APP_RESET_HANDLER_OFFSET    0x04U


/* --------------------------------------------------------------------------
 * Application Validation
 * -------------------------------------------------------------------------- */

uint8_t BL_Jump_IsApplicationValid(void)
{
    uint32_t stack_pointer;
    uint32_t reset_handler;

    /*
     * Application Vector Table
     *
     * Offset 0x00 -> Initial MSP
     * Offset 0x04 -> Reset Handler
     */

    stack_pointer =
        *(volatile uint32_t *)(BL_APP_START_ADDRESS +
                               BL_APP_STACK_POINTER_OFFSET);

    reset_handler =
        *(volatile uint32_t *)(BL_APP_START_ADDRESS +
                               BL_APP_RESET_HANDLER_OFFSET);


    /* Check SRAM address */

    if ((stack_pointer < SRAM_BASE) ||
        (stack_pointer >= (SRAM_BASE + SRAM_SIZE)))
    {
        return 0U;
    }


    /* Reset_Handler must be Thumb address */

    if ((reset_handler & 0x01U) == 0U)
    {
        return 0U;
    }


    /* Check Reset_Handler is inside Flash */

    if ((reset_handler < BL_APP_START_ADDRESS) ||
        (reset_handler >= (FLASH_BASE + FLASH_SIZE)))
    {
        return 0U;
    }


    return 1U;
}


/* --------------------------------------------------------------------------
 * Jump To Application
 * -------------------------------------------------------------------------- */

void BL_Jump_ToApplication(void)
{
    uint32_t app_stack_pointer;
    uint32_t app_reset_handler;

    void (*app_reset_handler_function)(void);


    /*
     * Read Application Vector Table
     */

    app_stack_pointer =
        *(volatile uint32_t *)(BL_APP_START_ADDRESS + 0x00U);

    app_reset_handler =
        *(volatile uint32_t *)(BL_APP_START_ADDRESS + 0x04U);


    /*
     * Disable Global Interrupts
     */

    __disable_irq();


    /*
     * Disable SysTick
     */

    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL  = 0U;


    /*
     * Disable NVIC Interrupts
     */

    for (uint32_t i = 0U;
         i < (sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]));
         i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }


    /*
     * Bootloader Peripheral DeInit
     *
     * Add your peripheral deinitialization here.
     *
     * Example:
     *
     * Bootloader_UART_DeInit();
     * GPIO_DeInit();
     *
     */


    /*
     * Relocate Vector Table
     */

    SCB->VTOR = BL_APP_START_ADDRESS;

    __DSB();
    __ISB();


    /*
     * Set Application Main Stack Pointer
     */

    __set_MSP(app_stack_pointer);

    __DSB();
    __ISB();


    /*
     * Get Application Reset Handler
     */

    app_reset_handler_function =
        (void (*)(void))app_reset_handler;


    /*
     * Jump To Application
     */

    app_reset_handler_function();


    /*
     * Should Never Return
     */

    while (1)
    {
    }
}