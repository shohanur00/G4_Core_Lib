#ifndef TIMECORE_H
#define TIMECORE_H

#include <stdint.h>

void TimeCore_Struct_Init(void);
void TimeCore_Init(void);
uint32_t TimeCore_GetMs(void);

uint8_t TimeCore_CreateTimer(uint32_t duration_ms);
void TimeCore_ResetTimer(uint8_t timer_id);
void TimeCore_PauseTimer(uint8_t timer_id);
void TimeCore_ResumeTimer(uint8_t timer_id);
void Timecore_StartTimer(uint8_t timer_id);
void TimeCore_StopTimer(uint8_t timer_id);
void TimeCore_ResetAllTimers(void);
void TimeCore_SetDurationSecurely(uint8_t timer_id, uint32_t duration_ms);
void TimeCore_SetDurationForcefully(uint8_t timer_id, uint32_t duration_ms);


uint8_t TimeCore_IsExpired(uint8_t timer_id);
uint32_t TimeCore_GetRemainingTime(uint8_t timer_id);
uint32_t TimeCore_GetElapsedTime(uint8_t timer_id);
uint8_t TimeCore_IsRunning(uint8_t timer_id);

void TimeCore_SafeDeleteTimer(uint8_t timer_id);
void TimeCore_ForceDeleteTimer(uint8_t timer_id);
void TimeCore_MainLoop(void);

#endif /* TIMECORE_H */
