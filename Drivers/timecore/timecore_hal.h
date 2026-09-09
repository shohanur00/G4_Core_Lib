#ifndef TIMECORE_HAL_H
#define TIMECORE_HAL_H

#include <stdint.h>

void TimeCore_HAL_Init(void);
void TimeCore_HAL_ClearUpdateFlag(void);
uint8_t TimeCore_HAL_GetUpdateFlag(void);
void TimeCore_HAL_SetUpdateFlag(void);
uint32_t TimeCore_HAL_GetMs(void);

#endif