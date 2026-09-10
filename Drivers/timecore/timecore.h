/**
 * @file timecore.h
 * @brief TimeCore - A lightweight embedded timer management library
 * @details Provides timer creation, management, and scheduling capabilities
 *          for embedded systems. Supports multiple concurrent timers with
 *          pause/resume, expiration checking, and time tracking features.
 * @author [Md. Shohanur Rahman]
 * @github [https://github.com/shohanur00/G4_Core_Lib/tree/main/Drivers/timecore]
 * @version 1.0.0
 * @date [09/09/2026]
 * @copyright [Copyright All rights reserved]
 */

#ifndef TIMECORE_H
#define TIMECORE_H

#include <stdint.h>

/**
 * @brief Initialize TimeCore module
 */
void TimeCore_Init(void);

/**
 * @brief Get current system time in milliseconds
 * @return Current time in milliseconds
 */
uint32_t TimeCore_GetMs(void);

/**
 * @brief Create a new timer with specified duration
 * @param duration_ms Timer duration in milliseconds
 * @return Timer ID (1-based), or 0 if no timers available
 */
uint8_t TimeCore_CreateTimer(uint32_t duration_ms);

/**
 * @brief Start a created timer
 * @param timer_id Timer ID returned from TimeCore_CreateTimer
 */
void Timecore_StartTimer(uint8_t timer_id);

/**
 * @brief Stop a running timer
 * @param timer_id Timer ID
 */
void TimeCore_StopTimer(uint8_t timer_id);

/**
 * @brief Pause a running timer
 * @param timer_id Timer ID
 */
void TimeCore_PauseTimer(uint8_t timer_id);

/**
 * @brief Resume a paused timer
 * @param timer_id Timer ID
 */
void TimeCore_ResumeTimer(uint8_t timer_id);

/**
 * @brief Reset a timer to its initial period
 * @param timer_id Timer ID
 */
void TimeCore_ResetTimer(uint8_t timer_id);

/**
 * @brief Reset all timers
 */
void TimeCore_ResetAllTimers(void);

/**
 * @brief Check if timer has expired
 * @param timer_id Timer ID
 * @return 1 if expired, 0 otherwise
 */
uint8_t TimeCore_IsExpired(uint8_t timer_id);

/**
 * @brief Check for continuous expiration events
 * @param timer_id Timer ID
 * @return 1 if expired, 0 otherwise
 */
uint8_t TimeCore_ContinousExpiredEvent(uint8_t timer_id);

/**
 * @brief Get remaining time for a timer
 * @param timer_id Timer ID
 * @return Remaining time in milliseconds
 */
uint32_t TimeCore_GetRemainingTime(uint8_t timer_id);

/**
 * @brief Get elapsed time for a timer
 * @param timer_id Timer ID
 * @return Elapsed time in milliseconds
 */
uint32_t TimeCore_GetElapsedTime(uint8_t timer_id);

/**
 * @brief Check if timer is running
 * @param timer_id Timer ID
 * @return 1 if running, 0 otherwise
 */
uint8_t TimeCore_IsRunning(uint8_t timer_id);

/**
 * @brief Check if timer is paused
 * @param timer_id Timer ID
 * @return 1 if paused, 0 otherwise
 */
uint8_t TimeCore_IsPaused(uint8_t timer_id);

/**
 * @brief Check if timer is allocated
 * @param timer_id Timer ID
 * @return 1 if allocated, 0 otherwise
 */
uint8_t TimeCore_IsAllocated(uint8_t timer_id);

/**
 * @brief Safely delete an expired timer
 * @param timer_id Timer ID
 */
void TimeCore_SafeDeleteTimer(uint8_t timer_id);

/**
 * @brief Force delete a timer
 * @param timer_id Timer ID
 */
void TimeCore_ForceDeleteTimer(uint8_t timer_id);

/**
 * @brief Set timer duration securely
 * @param timer_id Timer ID
 * @param duration_ms New duration in milliseconds
 */
void TimeCore_SetDurationSecurely(uint8_t timer_id, uint32_t duration_ms);

/**
 * @brief Set timer duration forcefully
 * @param timer_id Timer ID
 * @param duration_ms New duration in milliseconds
 */
void TimeCore_SetDurationForcefully(uint8_t timer_id, uint32_t duration_ms);

/**
 * @brief Main loop handler for timer updates
 */
void TimeCore_MainLoop(void);

/**
 * @brief Test function for timer functionality
 */

void TimeCore_Test(void);

#endif // TIMECORE_H