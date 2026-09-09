/* Application module implementation. */
#include "app.h"
#include "gpio/gpio.h"
#include "stm32g431xx.h"
#include "systemclock/systemclock.h"
#include "gpio/gpio.h"
#include "board.h"
#include "timecore/timecore.h"

uint8_t LED_timer; // Define the GPIO pin for the LED

void App_Setup(void)
{
    SystemClock_Init();
    GPIO_Init();
    TimeCore_Init();
    LED_timer = TimeCore_CreateTimer(1000); // Create a timer for 1000 ms
    Timecore_StartTimer(LED_timer);
    // Implementation for application setup
}


void App_Loop(void)
{   
    if(TimeCore_IsExpired(LED_timer))
    {
        TimeCore_ResetTimer(LED_timer); // Reset the timer for the next cycle
        Timecore_StartTimer(LED_timer); // Start the timer for the next cycle
        GPIO_Toggle(GPIO_LED); // Toggle the LED state
    }
    TimeCore_MainLoop(); // Call the main loop function for time-based tasks
    // Implementation for application loop
}