#include "logger_hal.h"
#include "board.h"


#if LOG_USE_UART
    #include "../backends/uart/logger_uart.h"
#endif

#if LOG_USE_FLASH
    #include "../Backends/Flash/logger_flash.h"
#endif

#if LOG_USE_USB
    #include "../Backends/USB/logger_usb.h"
#endif

#if LOG_USE_RTT
    #include "../Backends/RTT/logger_rtt.h"
#endif


void LOG_HAL_Init(void)
{
    #if LOG_USE_UART
        LOG_UART_Init();
    #endif

    #if LOG_USE_FLASH
        LOG_FLASH_Init();
    #endif

    #if LOG_USE_USB
        LOG_USB_Init();
    #endif

    #if LOG_USE_RTT
        LOG_RTT_Init();
    #endif
}


void LOG_HAL_Deinit(void)
{
    #if LOG_USE_UART
        LOG_UART_Deinit();
    #endif

    #if LOG_USE_FLASH
        LOG_FLASH_Deinit();
    #endif

    #if LOG_USE_USB
        LOG_USB_Deinit();
    #endif

    #if LOG_USE_RTT
        LOG_RTT_Deinit();
    #endif
}


void LOG_HAL_Write(
    const char *buffer,
    size_t length
)
{
    #if LOG_USE_UART
        LOG_UART_Write(buffer, length);
    #endif

    #if LOG_USE_FLASH
        LOG_FLASH_Write(buffer, length);
    #endif

    #if LOG_USE_USB
        LOG_USB_Write(buffer, length);
    #endif

    #if LOG_USE_RTT
        LOG_RTT_Write(buffer, length);
    #endif
}