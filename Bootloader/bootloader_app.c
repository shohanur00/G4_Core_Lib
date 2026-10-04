#include "bootloader_app.h"
#include "stm32g431xx.h"
#include "systemclock/systemclock.h"
#include "gpio/gpio.h"
#include "board.h"
#include "timecore/timecore.h"
#include "transport/uart/bootloader_uart.h"
// #include "tests/bl_protocol_test.h"
// #include "tests/bl_flash_test.h"
#include "logger/frontend/logger.h"
// #include "version.h"


static uint8_t LED_timer; // Define the GPIO pin for the LED


void Bootloader_App_Setup(void)
{
    /* Bootloader application setup */
    SystemClock_Init();
    GPIO_Init();
    TimeCore_Init();
    LOG_Init();
    // Version_LOG();
    Bootloader_UART_Init();
    LED_timer = TimeCore_CreateTimer(200); // Create a timer for 200 ms
    TimeCore_SetDurationSecurely(LED_timer, 1000); // Set the timer duration to 5000 ms
    TimeCore_StartTimer(LED_timer);
}

void Bootloader_App_Loop(void)
{
    /* Bootloader application loop */
    if(TimeCore_ContinousExpiredEvent(LED_timer))
    {
        GPIO_Toggle(GPIO_LED); // Toggle the LED state
        //BL_Protocol_Test();
        //BL_Flash_Test();
        // LOG_Disable();     
    }

    TimeCore_MainLoop(); // Call the main loop function for time-based tasks
    LOG_MainLoop(TimeCore_GetTick());
    // Implementation for application loop
}