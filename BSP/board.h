#ifndef BOARD_H
#define BOARD_H


#if BOOTLOADER == 1U
   #include "board_bootloader.h"
#else
   #include "board_firmware.h"
#endif

/* ============================================================================
 * Board Information
 * ========================================================================== */

#define BOARD_NAME                  "STM32G431CBT6"
#define BOARD_VERSION               "1.0"


#endif /* BOARD_H */