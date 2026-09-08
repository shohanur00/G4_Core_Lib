/* Application module implementation. */
#include "app.h"
#include "gpio/gpio.h"
#include "stm32g431xx.h"
#include "systemclock/systemclock.h"
#include "gpio/gpio.h"
#include "board.h"


void App_Setup(void)
{
    SystemClock_Init();
    GPIO_Init();
    // Implementation for application setup
}


void App_Loop(void)
{   
    GPIO_Toggle(GPIO_LED);
    for (volatile uint32_t i = 0; i < 100000; i++); // Simple delay loop
    // Implementation for application loop
}