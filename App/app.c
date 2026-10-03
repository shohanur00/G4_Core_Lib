/* Application module implementation. */
#include "app.h"
#include "stm32g431xx.h"
#include "systemclock/systemclock.h"
#include "gpio/gpio.h"
#include "board.h"
#include "timecore/timecore.h"
#include "logger/frontend/logger.h"
#include "version.h"
#include "transport/uart/bootloader_uart.h"
#include "tests/bl_protocol_test.h"
#include "tests/bl_flash_test.h"


uint8_t LED_timer; // Define the GPIO pin for the LED

void App_Setup(void)
{
    SystemClock_Init();
    GPIO_Init();
    TimeCore_Init();
    LOG_Init();
    Version_LOG();
    Bootloader_UART_Init();
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
        // LOG_DEBUG(LOG_MODULE_SYSTEM,"Current: %d",20);
        // LOG_INFO(LOG_MODULE_SYSTEM,"SYSTEM %s","INIT");
        // LOG_WARNING(LOG_MODULE_SYSTEM,"WARNING!");
        // LOG_ERROR(LOG_MODULE_SYSTEM,"OVER Temperature!");
        // LOG_CRITICAL(LOG_MODULE_SYSTEM,"System Failure!");
        // LOG_UART_Write("Hello", 5);
        //Bootloader_UART_Write("Hello\r\n", 7U);
        // BL_Protocol_Test();
        BL_Flash_Test();
        LOG_Disable();

        
    }

    uint8_t data;

    // if (Bootloader_UART_ReadByte(&data))
    // {
    //     //Bootloader_UART_Write(&data, 1U); // Echo the received byte back
    //     /* One byte received */
    // }
    TimeCore_MainLoop(); // Call the main loop function for time-based tasks
    LOG_MainLoop(TimeCore_GetTick());
    // Implementation for application loop
}