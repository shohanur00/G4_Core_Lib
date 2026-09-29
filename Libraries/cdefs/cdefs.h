
#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
// #include <stddef.h>

/* ============================================================================
 * Common Definitions
 * ========================================================================== */

#define ENABLE          1U
#define DISABLE         0U

#define HIGH            1U
#define LOW             0U

#define SET             1U
#define RESET           0U

#define ON              1U
#define OFF             0U

#define TRUE            1U
#define FALSE           0U

#ifndef NULL
#define NULL            ((void *)0)
#endif

/* ============================================================================
 * Common Utilities
 * ========================================================================== */

#define UNUSED(x)       ((void)(x))
#define ARRAY_SIZE(x)   (sizeof(x) / sizeof((x)[0]))


/* ============================================================================
 * UART Utilities
 * ========================================================================== */


 

#endif /* COMMON_H */