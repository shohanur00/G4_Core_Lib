/* Application module implementation. */
#include "app.h"

#if BOOTLOADER == 1U

#include "bootloader_app.h"

#else

#include "firmware.h"

#endif

void App_Setup(void)
{

    #if BOOTLOADER == 1U
       Bootloader_App_Setup();
    #else
       Firmware_Setup();
    #endif

}


void App_Loop(void)
{   
    
    #if BOOTLOADER == 1U
       Bootloader_App_Loop();
    #else
       Firmware_Loop();
    #endif

}