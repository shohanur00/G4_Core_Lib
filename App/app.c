/* Application module implementation. */
#include "app.h"
#include "stm32g431xx.h"
#include "systemclock.h"
#include "gpio.h"
#include "board.h"
#include "timecore.h"
#include "logger.h"

uint8_t LED_timer; // Define the GPIO pin for the LED

void App_Setup(void)
{
    SystemClock_Init();
    GPIO_Init();
    TimeCore_Init();
    LOG_Init();
    LED_timer = TimeCore_CreateTimer(200); // Create a timer for 200 ms
    TimeCore_SetDurationSecurely(LED_timer, 1000); // Set the timer duration to 5000 ms
    TimeCore_StartTimer(LED_timer);
    // Implementation for application setup
}


void App_Loop(void)
{   
    if(TimeCore_ContinousExpiredEvent(LED_timer))
    {
        //TimeCore_ResetTimer(LED_timer); // Reset the timer for the next cycle
        // TimeCore_StartTimer(LED_timer); // Start the timer for the next cycle
        GPIO_Toggle(GPIO_LED); // Toggle the LED state
        LOG_DEBUG(LOG_MODULE_SYSTEM,"Hello: %d",20);
    }
    TimeCore_MainLoop(); // Call the main loop function for time-based tasks
    LOG_MainLoop(TimeCore_GetTick());
    // Implementation for application loop
}