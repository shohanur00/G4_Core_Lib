#ifndef BL_JUMP_H
#define BL_JUMP_H

#include <stdint.h>


/* --------------------------------------------------------------------------
 * API
 * -------------------------------------------------------------------------- */

uint8_t BL_Jump_IsApplicationValid(void);

void BL_Jump_ToApplication(void);

#endif /* BL_JUMP_H */