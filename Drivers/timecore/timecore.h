/**
 * @file    timecore.h
 * @brief   TimeCore - Lightweight Embedded Timer Management Library
 *
 * @details
 * Provides timer creation, management, scheduling, and time-tracking
 * capabilities for embedded systems. Supports multiple concurrent timers
 * with start, stop, pause, resume, reset, expiration events, and
 * timer lifecycle management.
 *
 * @author  [Md. Shohanur Rahman]
 * @github  [https://github.com/shohanur00/G4_Core_Lib/tree/main/Drivers/timecore]
 * @version [1.0.0]
 * @date    [09/09/2026]
 * @copyright [Copyright All rights reserved]
 */

#ifndef TIMECORE_H
#define TIMECORE_H

#include <stdint.h>


/* ============================================================================
 * Module Initialization
 * ========================================================================== */

/**
 * @brief Initialize the TimeCore module.
 */
void TimeCore_Init(void);

/**
 * @brief Deinitialize the TimeCore module.
 */
void TimeCore_Deinit(void);


/* ============================================================================
 * System Time
 * ========================================================================== */

/**
 * @brief Get the current system time.
 *
 * @return Current system time in milliseconds.
 */
uint32_t TimeCore_GetMs(void);


/* ============================================================================
 * Timer Creation & Deletion
 * ========================================================================== */

/**
 * @brief Create a new timer with the specified duration.
 *
 * @param duration_ms Timer duration in milliseconds.
 *
 * @return
 * Timer ID (1-based), or 0 if no timer slot is available.
 */
uint8_t TimeCore_CreateTimer(uint32_t duration_ms);

/**
 * @brief Safely delete an expired timer.
 *
 * @param timer_id Timer ID.
 */
void TimeCore_SafeDeleteTimer(uint8_t timer_id);

/**
 * @brief Forcefully delete a timer.
 *
 * @param timer_id Timer ID.
 */
void TimeCore_ForceDeleteTimer(uint8_t timer_id);

/**
 * @brief Delete all allocated timers.
 */
void TimeCore_DeleteAllTimers(void);


/* ============================================================================
 * Timer Control
 * ========================================================================== */

/**
 * @brief Start a created timer.
 *
 * @param timer_id Timer ID returned by TimeCore_CreateTimer().
 */
void TimeCore_StartTimer(uint8_t timer_id);

/**
 * @brief Stop a running timer.
 *
 * @param timer_id Timer ID.
 */
void TimeCore_StopTimer(uint8_t timer_id);

/**
 * @brief Pause a running timer.
 *
 * @param timer_id Timer ID.
 */
void TimeCore_PauseTimer(uint8_t timer_id);

/**
 * @brief Resume a paused timer.
 *
 * @param timer_id Timer ID.
 */
void TimeCore_ResumeTimer(uint8_t timer_id);

/**
 * @brief Reset a timer to its initial duration.
 *
 * @param timer_id Timer ID.
 */
void TimeCore_ResetTimer(uint8_t timer_id);

/**
 * @brief Reset all timers.
 */
void TimeCore_ResetAllTimers(void);


/* ============================================================================
 * Timer Events
 * ========================================================================== */

/**
 * @brief Check whether a timer has expired.
 *
 * @param timer_id Timer ID.
 *
 * @return
 * 1 if the timer is expired, otherwise 0.
 */
uint8_t TimeCore_IsExpired(uint8_t timer_id);

/**
 * @brief Check for a continuous expiration event.
 *
 * @param timer_id Timer ID.
 *
 * @return
 * 1 when the timer expiration event occurs, otherwise 0.
 */
uint8_t TimeCore_ContinuousExpiredEvent(uint8_t timer_id);

/**
 * @brief Check for a one-shot expiration event.
 *
 * @param timer_id Timer ID.
 *
 * @return
 * 1 when the timer expiration event occurs, otherwise 0.
 */
uint8_t TimeCore_OneShotExpiredEvent(uint8_t timer_id);


/* ============================================================================
 * Timer Information
 * ========================================================================== */

/**
 * @brief Get the remaining time of a timer.
 *
 * @param timer_id Timer ID.
 *
 * @return Remaining time in milliseconds.
 */
uint32_t TimeCore_GetRemainingTime(uint8_t timer_id);

/**
 * @brief Get the elapsed time of a timer.
 *
 * @param timer_id Timer ID.
 *
 * @return Elapsed time in milliseconds.
 */
uint32_t TimeCore_GetElapsedTime(uint8_t timer_id);

/**
 * @brief Check whether a timer is currently running.
 *
 * @param timer_id Timer ID.
 *
 * @return
 * 1 if running, otherwise 0.
 */
uint8_t TimeCore_IsRunning(uint8_t timer_id);

/**
 * @brief Check whether a timer is currently paused.
 *
 * @param timer_id Timer ID.
 *
 * @return
 * 1 if paused, otherwise 0.
 */
uint8_t TimeCore_IsPaused(uint8_t timer_id);

/**
 * @brief Check whether a timer is allocated.
 *
 * @param timer_id Timer ID.
 *
 * @return
 * 1 if allocated, otherwise 0.
 */
uint8_t TimeCore_IsAllocated(uint8_t timer_id);


/* ============================================================================
 * Timer Configuration
 * ========================================================================== */

/**
 * @brief Set timer duration securely.
 *
 * @param timer_id    Timer ID.
 * @param duration_ms New timer duration in milliseconds.
 */
void TimeCore_SetDurationSecurely(uint8_t timer_id, uint32_t duration_ms);

/**
 * @brief Set timer duration forcefully.
 *
 * @param timer_id    Timer ID.
 * @param duration_ms New timer duration in milliseconds.
 */
void TimeCore_SetDurationForcefully(uint8_t timer_id, uint32_t duration_ms);


/* ============================================================================
 * Main Processing
 * ========================================================================== */

/**
 * @brief Process TimeCore timer updates.
 *
 * @note This function should be called periodically from the main loop
 *       or scheduler.
 */
void TimeCore_MainLoop(void);


/* ============================================================================
 * Testing & Diagnostics
 * ========================================================================== */

/**
 * @brief Run the TimeCore timer functionality test.
 *
 * @note Intended for development and debugging purposes.
 */
void TimeCore_Test(void);


#endif /* TIMECORE_H */

