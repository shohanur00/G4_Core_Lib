#ifndef SYSTEMCLOCK_H
#define SYSTEMCLOCK_H

#include <stdint.h>

#define SYSTEMCLOCK_FREQ_HZ    170000000UL

void SystemClock_Init(void);
uint32_t SystemClock_GetFrequency(void);

#endif