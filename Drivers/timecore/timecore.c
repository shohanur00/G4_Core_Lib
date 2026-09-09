#include "timecore.h"
#include "timecore_hal.h"
#include <stdint.h>


#define TIMECORE_MAX_TIMERS    5U

typedef enum
{
    TIMECORE_TIMER_INVALID = 0U,
    TIMECORE_TIMER_CREATED,
    TIMECORE_TIMER_RUNNING,
    TIMECORE_TIMER_PAUSED,
    TIMECORE_TIMER_EXPIRED,
    TIMECORE_TIMER_STOPPED

} TimeCore_TimerState_t;


typedef struct
{
    uint8_t                 id;
    uint8_t                 allocated;

    uint32_t                period_ms;
    volatile uint32_t       remaining_ms;

    TimeCore_TimerState_t   state;

} TimeCore_Timer_t;



static TimeCore_Timer_t timers[TIMECORE_MAX_TIMERS];


void TimeCore_Struct_Init(void)
{
    for (uint8_t i = 0; i < TIMECORE_MAX_TIMERS; i++)
    {
        timers[i].id = i;
        timers[i].allocated = 0U;
        timers[i].period_ms = 0U;
        timers[i].remaining_ms = 0U;
        timers[i].state = TIMECORE_TIMER_INVALID;
    }
}

void TimeCore_Init(void)
{
    TimeCore_Struct_Init();
    TimeCore_HAL_Init();
}

uint32_t TimeCore_GetMs(void)
{
    return TimeCore_HAL_GetMs();
}

uint8_t TimeCore_CreateTimer(uint32_t duration_ms)
{
    for (uint8_t i = 0; i < TIMECORE_MAX_TIMERS; i++)
    { 
        if (timers[i].state == TIMECORE_TIMER_INVALID)
        {
            timers[i].period_ms = duration_ms;
            timers[i].remaining_ms = duration_ms;
            timers[i].state = TIMECORE_TIMER_CREATED;
            return timers[i].id+1U; // Return timer ID (1-based)
        }
    }
    
    return 0U; // No available timers
}

void TimeCore_ResetTimer(uint8_t timer_id)
{
    if ((timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS ) && (timers[timer_id - 1U].state != TIMECORE_TIMER_INVALID))
    {
        timers[timer_id - 1U].state = TIMECORE_TIMER_CREATED;
        timers[timer_id - 1U].remaining_ms = timers[timer_id - 1U].period_ms; // Reset remaining time
    }

}


void TimeCore_PauseTimer(uint8_t timer_id)
{
    if ((timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS ) && (timers[timer_id - 1U].state == TIMECORE_TIMER_RUNNING))
    {
        timers[timer_id - 1U].state = TIMECORE_TIMER_PAUSED;
    }
}


void TimeCore_ResumeTimer(uint8_t timer_id)
{
    if ((timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS ) && (timers[timer_id - 1U].state == TIMECORE_TIMER_PAUSED))
    {
        timers[timer_id - 1U].state = TIMECORE_TIMER_RUNNING; 
    }
}


void Timecore_StartTimer(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state == TIMECORE_TIMER_CREATED)
    {
        timers[timer_id - 1U].state = TIMECORE_TIMER_RUNNING; // Start the timer
    }

}


void TimeCore_StopTimer(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state != TIMECORE_TIMER_INVALID)
    {
        timers[timer_id - 1U].state = TIMECORE_TIMER_STOPPED; // Stop the timer
    }

}


void TimeCore_ResetAllTimers(void)
{
    for (uint8_t i = 0; i < TIMECORE_MAX_TIMERS; i++)
    {
        timers[i].state = TIMECORE_TIMER_CREATED; // Reset all timer states
        timers[i].remaining_ms = 0U; // Reset all timers to 0
    }
}


uint8_t TimeCore_IsExpired(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state != TIMECORE_TIMER_INVALID)
    {
        if (timers[timer_id - 1U].remaining_ms == 0U)
        {
            timers[timer_id - 1U].state = TIMECORE_TIMER_EXPIRED;
            return 1U;
        }
    }
    return 0U; // Invalid timer ID returns 0
}


uint8_t TimeCore_ContinousExpiredEvent(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state != TIMECORE_TIMER_INVALID)
    {
        if (timers[timer_id - 1U].remaining_ms == 0U)
        {
            timers[timer_id - 1U].remaining_ms = timers[timer_id - 1U].period_ms; // Reset remaining time for continuous event
            timers[timer_id - 1U].state = TIMECORE_TIMER_RUNNING; // Mark as running
            return 1U;
        }
    }
    return 0U; // Invalid timer ID returns 0
}


uint32_t TimeCore_GetRemainingTime(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state != TIMECORE_TIMER_INVALID)
    {
        return timers[timer_id - 1U].remaining_ms; // Return remaining time in ms
    }
    return 0U; // Invalid timer ID returns 0
}

uint32_t TimeCore_GetElapsedTime(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state != TIMECORE_TIMER_INVALID)
    {
        return (timers[timer_id - 1U].remaining_ms == 0U) ? timers[timer_id - 1U].period_ms : (timers[timer_id - 1U].period_ms - timers[timer_id - 1U].remaining_ms); // Return elapsed time in ms
    }
    return 0U; // Invalid timer ID returns 0
}

uint8_t TimeCore_IsRunning(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state == TIMECORE_TIMER_RUNNING)
    {
        return 1U; // Timer is running
    }
    return 0U; // Timer is not running
}

uint8_t TimeCore_IsPaused(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state == TIMECORE_TIMER_PAUSED)
    {
        return 1U; // Timer is paused
    }
    return 0U; // Timer is not paused
}

uint8_t TimeCore_IsAllocated(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state != TIMECORE_TIMER_INVALID)
    {
        return 1U; // Timer is allocated
    }
    return 0U; // Timer is not allocated
}

void TimeCore_SafeDeleteTimer(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state == TIMECORE_TIMER_EXPIRED && (timers[timer_id - 1U].state != TIMERCORE_TIMER_INVALID))
    {
        timers[timer_id - 1U].state = TIMECORE_TIMER_INVALID; // Mark the timer as invalid
        timers[timer_id - 1U].remaining_ms = 0U; // Reset remaining time
        timers[timer_id - 1U].period_ms = 0U; // Reset period time
    }
}


void TimeCore_ForceDeleteTimer(uint8_t timer_id)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state != TIMECORE_TIMER_INVALID)
    {
        timers[timer_id - 1U].state = TIMECORE_TIMER_INVALID; // Mark the timer as invalid
        timers[timer_id - 1U].remaining_ms = 0U; // Reset remaining time
        timers[timer_id - 1U].period_ms = 0U; // Reset period time
    }
}


void TimeCore_SetDurationSecurely(uint8_t timer_id, uint32_t duration_ms)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && (timers[timer_id - 1U].state == TIMECORE_TIMER_EXPIRED) && (timers[timer_id - 1U].state != TIMERCORE_TIMER_INVALID))
    {
        // Pause the timer if it's running
        TimeCore_PauseTimer(timer_id);

        // Set the new duration
        timers[timer_id - 1U].period_ms = duration_ms;
        timers[timer_id - 1U].remaining_ms = duration_ms;

        // Resume the timer if it was running before
        if (timers[timer_id - 1U].state == TIMECORE_TIMER_PAUSED)
        {
            TimeCore_ResumeTimer(timer_id);
        }
    }
}


void TimeCore_SetDurationForcefully(uint8_t timer_id, uint32_t duration_ms)
{
    if (timer_id > 0U && timer_id <= TIMECORE_MAX_TIMERS && timers[timer_id - 1U].state != TIMECORE_TIMER_INVALID)
    {
        // Set the new duration
        timers[timer_id - 1U].period_ms = duration_ms;
        timers[timer_id - 1U].remaining_ms = duration_ms;
    }
}


void TimeCore_MainLoop(void)
{   
    if (TimeCore_HAL_GetUpdateFlag())
    {
        // Clear the update flag
        TimeCore_HAL_ClearUpdateFlag();
        for (uint8_t i = 0; i < TIMECORE_MAX_TIMERS; i++)
        {
            if (timers[i].state == TIMECORE_TIMER_RUNNING)
            {
                if (timers[i].remaining_ms > 0U && timers[i].remaining_ms <= timers[i].period_ms)
                {
                    timers[i].remaining_ms--;
                }
            }
        }
        // Perform time-based tasks here
        // For example, you can call a function to handle periodic events
        // or update timers, etc.
    }
    // This function can be used to handle any time-based tasks if needed.
    // Currently, it does not perform any operations.
}
