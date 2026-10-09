#include "bootloader_app.h"

#include <stdint.h>

#include "cdefs/cdefs.h"
#include "stm32g431xx.h"

#include "board.h"
#include "systemclock/systemclock.h"
#include "gpio/gpio.h"
#include "timecore/timecore.h"
#include "logger/frontend/logger.h"
#include "jump/bl_jump.h"
#include "core/bootloader.h"



/* ============================================================================
 * LED State
 * ========================================================================== */

typedef enum
{
    LED_CONNECTED = 0,
    LED_PROGRAMMING,
    LED_ERROR,
    LED_NOT_CONNECTED

} LED_State_t;


/* ============================================================================
 * Private Variables
 * ========================================================================== */

static uint8_t LED_timer;
static uint8_t jump_timer;
static uint8_t timeout;

static LED_State_t state = LED_NOT_CONNECTED;


/* ============================================================================
 * Bootloader Application Deinitialization
 * ========================================================================== */

void Bootloader_App_Deinit(void)
{
    /* Deinitialize bootloader application services */
    LOG_Deinit();
    TimeCore_Deinit();
    GPIO_DeInit();
}


/* ============================================================================
 * Bootloader Application Setup
 * ========================================================================== */

void Bootloader_App_Setup(void)
{
    /* System initialization */
    SystemClock_Init();
    GPIO_Init();
    TimeCore_Init();
    LOG_Init();

    /* Bootloader initialization */
    Bootloader_Init();

    /* Create application timers */
    LED_timer = TimeCore_CreateTimer(
        BL_LED_NOT_CONNECTED_BLINK_TIME
    );

    jump_timer = TimeCore_CreateTimer(
        BL_STARTUP_WAIT_TIME
    );

    timeout = TimeCore_CreateTimer(
        BL_PROGRAMMING_TIMEOUT
    );

    /* Start required timers */
    TimeCore_StartTimer(LED_timer);
    TimeCore_StartTimer(jump_timer);
    TimeCore_StartTimer(timeout);

    /* Programming timeout remains inactive until required */
    TimeCore_PauseTimer(timeout);
}


/* ============================================================================
 * Bootloader Application Main Loop
 * ========================================================================== */

void Bootloader_App_Loop(void)
{
    /* ------------------------------------------------------------------------
     * Bootloader Communication
     * ---------------------------------------------------------------------- */

    Bootloader_Process();


    /* ------------------------------------------------------------------------
     * Initial Startup Wait
     *
     * Wait for PC synchronization during the initial bootloader period.
     *
     * If no PC connection is detected:
     *   - Validate the existing application.
     *   - Valid application   -> Jump to application.
     *   - Invalid application -> Remain in bootloader.
     *
     * If bootloader is already connected/programming:
     *   - Start the additional programming timeout.
     * ---------------------------------------------------------------------- */

    if (TimeCore_OneShotExpiredEvent(jump_timer))
    {
        if (Bootloader_GetState() == BL_STATE_WAIT_SYNC)
        {
            if (Bootloader_ValidateApplication() && BL_Jump_IsApplicationValid())
            {
                Bootloader_App_Deinit();
                BL_Jump_ToApplication();
            }
            else
            {
                /* No valid application.
                 * Remain in bootloader indefinitely.
                 */
                Bootloader_GetBackTo_Idle();

                TimeCore_SetDurationForcefully(
                    LED_timer,
                    BL_LED_ERROR_BLINK_TIME
                );

                state = LED_NOT_CONNECTED;
            }
        }
        else
        {
            /* Bootloader is connected/programming.
             * Allow additional time for firmware programming.
             */
            TimeCore_ResumeTimer(timeout);
        }
    }


    /* ------------------------------------------------------------------------
     * Firmware Update Completed
     *
     * Jump immediately after successful firmware validation.
     * Do not wait for the programming timeout.
     * ---------------------------------------------------------------------- */

    if (Bootloader_GetState() == BL_STATE_VALID)
    {
        Bootloader_App_Deinit();
        BL_Jump_ToApplication();
    }


    /* ------------------------------------------------------------------------
     * Additional Programming Timeout
     *
     * If firmware programming does not complete within the additional
     * timeout period, validate the existing application.
     *
     * Valid application   -> Jump to application.
     * Invalid application -> Remain in bootloader.
     * ---------------------------------------------------------------------- */

    if (TimeCore_OneShotExpiredEvent(timeout))
    {
        if (Bootloader_GetState() != BL_STATE_VALID)
        {
            if (Bootloader_ValidateApplication() && BL_Jump_IsApplicationValid())
            {
                Bootloader_App_Deinit();
                BL_Jump_ToApplication();
            }
            else
            {
                /* No valid application.
                 * Remain in bootloader indefinitely.
                 */
                Bootloader_GetBackTo_Idle();
                // GPIO_Write(GPIO_LED, HIGH);
                TimeCore_SetDurationForcefully(
                    LED_timer,
                    BL_LED_ERROR_BLINK_TIME
                );

                TimeCore_ResumeTimer(LED_timer);
                TimeCore_ResetTimer(timeout);
                TimeCore_StartTimer(timeout);

                state = LED_NOT_CONNECTED;
            }
        }
    }


    /* ------------------------------------------------------------------------
     * LED State: Connected
     * ---------------------------------------------------------------------- */

    if (Bootloader_GetState() == BL_STATE_CONNECTED)
    {
        if (state == LED_NOT_CONNECTED)
        {
            state = LED_CONNECTED;

            TimeCore_PauseTimer(LED_timer);

            GPIO_Write(
                GPIO_LED,
                LOW
            );
            TimeCore_ResumeTimer(timeout);
        }
    }


    /* ------------------------------------------------------------------------
     * LED State: Programming
     * ---------------------------------------------------------------------- */

    if (Bootloader_GetState() == BL_STATE_PROGRAMMING)
    {
        if (state == LED_CONNECTED)
        {
            state = LED_PROGRAMMING;

            TimeCore_ResumeTimer(LED_timer);

            TimeCore_SetDurationForcefully(
                LED_timer,
                BL_LED_PROGRAMMING_BLINK_TIME
            );
        }
    }


    /* ------------------------------------------------------------------------
     * LED Heartbeat
     * ---------------------------------------------------------------------- */

    if (TimeCore_ContinousExpiredEvent(LED_timer))
    {
        GPIO_Toggle(GPIO_LED);
    }


    /* ------------------------------------------------------------------------
     * Time Services
     * ---------------------------------------------------------------------- */

    TimeCore_MainLoop();


    /* ------------------------------------------------------------------------
     * Logger Processing
     * ---------------------------------------------------------------------- */

    LOG_MainLoop(
        TimeCore_GetTick()
    );
}
