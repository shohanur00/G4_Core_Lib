/* Application module implementation. */
#include "app.h"
#include "stm32g431xx.h"
#include "systemclock.h"
#include "gpio.h"
#include "board.h"
#include "timecore.h"
#include "logger.h"
#include "version.h"


uint8_t LED_timer; // Define the GPIO pin for the LED

void App_Setup(void)
{
    SystemClock_Init();
    GPIO_Init();
    TimeCore_Init();
    LOG_Init();
    Version_LOG();
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
        LOG_DEBUG(LOG_MODULE_SYSTEM,"Current: %d",20);
        LOG_INFO(LOG_MODULE_SYSTEM,"SYSTEM %s","INIT");
        LOG_WARNING(LOG_MODULE_SYSTEM,"WARNING!");
        LOG_ERROR(LOG_MODULE_SYSTEM,"OVER Temperature!");
        LOG_CRITICAL(LOG_MODULE_SYSTEM,"System Failure!");
        // LOG_UART_Write("Hello", 5);
        LOG_Disable();
    }
    TimeCore_MainLoop(); // Call the main loop function for time-based tasks
    LOG_MainLoop(TimeCore_GetTick());
    // Implementation for application loop
}