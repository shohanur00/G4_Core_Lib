#include "firmware.h"
#include "stm32g431xx.h"
#include "systemclock/systemclock.h"
#include "gpio/gpio.h"
#include "board.h"
#include "timecore/timecore.h"
#include "logger/frontend/logger.h"
#include "version.h"

/* --------------------------------------------------------------------------
 * Firmware Setup
 * -------------------------------------------------------------------------- */

static uint8_t LED_timer; // Define the GPIO pin for the LED

void Firmware_Setup(void)
{
    SystemClock_Init();

    GPIO_Init();

    TimeCore_Init();

    LOG_Init();

    Version_LOG();

    LED_timer = TimeCore_CreateTimer(200);

    TimeCore_SetDurationSecurely(LED_timer, 500);

    TimeCore_StartTimer(LED_timer);

}

/* --------------------------------------------------------------------------
 * Firmware Loop
 * -------------------------------------------------------------------------- */

void Firmware_Loop(void)
{
    /* Firmware main loop */
    // LOG_INFO(LOG_MODULE_SYSTEM,"MAIN_LOOP_STARTED!");
    if(TimeCore_ContinousExpiredEvent(LED_timer))
    {
        GPIO_Toggle(GPIO_LED); // Toggle the LED state
        // LOG_INFO(LOG_MODULE_SYSTEM,"Timer Working!");
        // LOG_DEBUG(LOG_MODULE_SYSTEM,"Current: %d",20);
        // LOG_INFO(LOG_MODULE_SYSTEM,"SYSTEM %s","INIT");
        // LOG_WARNING(LOG_MODULE_SYSTEM,"WARNING!");
        // LOG_ERROR(LOG_MODULE_SYSTEM,"OVER Temperature!");
        // LOG_CRITICAL(LOG_MODULE_SYSTEM,"System Failure!");
        // LOG_Disable();   
    }

    TimeCore_MainLoop();                                    // Call the main loop function for time-based tasks
    LOG_MainLoop(TimeCore_GetTick());
    
}