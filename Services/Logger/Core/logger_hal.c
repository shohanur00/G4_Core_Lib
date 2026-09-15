#include "logger_hal.h"



#if LOG_HAL_USE_UART
    #include "../Backends/UART/logger_uart.h"
#endif

#if LOG_HAL_USE_FLASH
    #include "../Backends/Flash/logger_flash.h"
#endif

#if LOG_HAL_USE_USB
    #include "../Backends/USB/logger_usb.h"
#endif

#if LOG_HAL_USE_RTT
    #include "../Backends/RTT/logger_rtt.h"
#endif


void LOG_HAL_Init(void)
{
    #if LOG_HAL_USE_UART
        LOG_UART_Init();
    #endif

    #if LOG_HAL_USE_FLASH
        LOG_FLASH_Init();
    #endif

    #if LOG_HAL_USE_USB
        LOG_USB_Init();
    #endif

    #if LOG_HAL_USE_RTT
        LOG_RTT_Init();
    #endif
}


void LOG_HAL_Write(
    const char *buffer,
    size_t length
)
{
    #if LOG_HAL_USE_UART
        LOG_UART_Write(buffer, length);
    #endif

    #if LOG_HAL_USE_FLASH
        LOG_FLASH_Write(buffer, length);
    #endif

    #if LOG_HAL_USE_USB
        LOG_USB_Write(buffer, length);
    #endif

    #if LOG_HAL_USE_RTT
        LOG_RTT_Write(buffer, length);
    #endif
}