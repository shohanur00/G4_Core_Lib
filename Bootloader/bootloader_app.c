#include "bootloader_app.h"
#include "stm32g431xx.h"
#include "systemclock/systemclock.h"
#include "gpio/gpio.h"
#include "board.h"
#include "timecore/timecore.h"
#include "tests/bl_flash_test.h"
#include "logger/frontend/logger.h"
#include "jump/bl_jump.h"
// #include "version.h"


static uint8_t LED_timer; // Define the GPIO pin for the LED
static uint8_t jump_timer;  



void Bootloader_App_Deinit(void){
    /* Bootloader application deinitialization */
    LOG_Deinit();
    TimeCore_Deinit();
    GPIO_DeInit();

}


void Bootloader_App_Setup(void)
{
    /* Bootloader application setup */
    SystemClock_Init();
    GPIO_Init();
    TimeCore_Init();
    LOG_Init();
    // Version_LOG();
    LED_timer = TimeCore_CreateTimer(200); // Create a timer for 200 ms
    jump_timer = TimeCore_CreateTimer(5000); // Create a timer for 5 seconds
    TimeCore_StartTimer(LED_timer);
    TimeCore_StartTimer(jump_timer);
}

void Bootloader_App_Loop(void)
{
    /* Bootloader application loop */
    if(TimeCore_ContinousExpiredEvent(LED_timer))
    {
        GPIO_Toggle(GPIO_LED); // Toggle the LED state
        //BL_Protocol_Test();
        //BL_Flash_Test();
        //LOG_Disable();   
        LOG_INFO(LOG_MODULE_SYSTEM,"Bootloader RUNNING!");  
    }

    if(TimeCore_OneShotExpiredEvent(jump_timer))
    {
        LOG_INFO(LOG_MODULE_SYSTEM,"Jumping to Application!\n\r");  
        Bootloader_App_Deinit();
        BL_Jump_ToApplication();
        // LOG_Disable();
    }
    TimeCore_MainLoop(); // Call the main loop function for time-based tasks
    LOG_MainLoop(TimeCore_GetTick());
    // Implementation for application loop
}