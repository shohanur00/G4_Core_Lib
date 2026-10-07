#ifndef BL_JUMP_H
#define BL_JUMP_H


/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>


/* ============================================================================
 * Application Validation
 * ========================================================================== */

/**
 * @brief Check whether the main application is valid.
 *
 * @return 1U Application is valid.
 * @return 0U Application is invalid.
 */
uint8_t BL_Jump_IsApplicationValid(void);


/* ============================================================================
 * Application Jump
 * ========================================================================== */

/**
 * @brief Jump from the bootloader to the main application.
 *
 * Transfers execution from the bootloader to the application's reset handler.
 */
void BL_Jump_ToApplication(void);


#endif /* BL_JUMP_H */

