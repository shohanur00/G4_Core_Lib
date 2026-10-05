#ifndef BL_PROTOCOL_TEST_H
#define BL_PROTOCOL_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../protocol/bl_protocol.h"


/* ============================================================================
 * Test Configuration
 * ========================================================================== */

/* --------------------------------------------------------------------------
 * Logger
 * -------------------------------------------------------------------------- */

#ifndef BL_PROTOCOL_TEST_LOG_MODULE
#define BL_PROTOCOL_TEST_LOG_MODULE             LOG_MODULE_BOOTLOADER
#endif


/* --------------------------------------------------------------------------
 * Command Test Configuration
 * -------------------------------------------------------------------------- */

#ifndef BL_PROTOCOL_TEST_VALID_COMMAND
#define BL_PROTOCOL_TEST_VALID_COMMAND          BL_CMD_FW_UPDATE_REQ
#endif

#ifndef BL_PROTOCOL_TEST_INVALID_COMMAND
#define BL_PROTOCOL_TEST_INVALID_COMMAND        BL_CMD_DEVICE_ID_REQ
#endif


/* --------------------------------------------------------------------------
 * Test Data Configuration
 * -------------------------------------------------------------------------- */

#ifndef BL_PROTOCOL_TEST_DATA_SIZE
#define BL_PROTOCOL_TEST_DATA_SIZE              (5U)
#endif

#ifndef BL_PROTOCOL_TEST_DATA_INIT
#define BL_PROTOCOL_TEST_DATA_INIT              \
{                                               \
    0x31U,                                      \
    0x12U,                                      \
    0x34U,                                      \
    0x56U,                                      \
    0x78U                                       \
}
#endif


/* --------------------------------------------------------------------------
 * All Commands Configuration
 *
 * Change this list when protocol commands change.
 * -------------------------------------------------------------------------- */

#ifndef BL_PROTOCOL_TEST_COMMAND_LIST
#define BL_PROTOCOL_TEST_COMMAND_LIST           \
{                                               \
    BL_CMD_SYNC_OBSERVED,                       \
    BL_CMD_FW_UPDATE_REQ,                       \
    BL_CMD_FW_UPDATE_RES,                       \
    BL_CMD_DEVICE_ID_REQ,                       \
    BL_CMD_DEVICE_ID_RES,                       \
    BL_CMD_FW_SIZE,                             \
    BL_CMD_FW_OVER_SIZE,                        \
    BL_CMD_READY_FOR_DATA,                      \
    BL_CMD_UPDATE_SUCCESSFUL,                   \
    BL_CMD_ACK,                                 \
    BL_CMD_NACK,                                \
    BL_CMD_RETX                                 \
}
#endif


/* ============================================================================
 * Individual Test Enable / Disable
 *
 * Set to 0 to disable a particular test.
 * ========================================================================== */

#ifndef BL_PROTOCOL_TEST_ENABLE_COMMAND
#define BL_PROTOCOL_TEST_ENABLE_COMMAND         (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_DATA
#define BL_PROTOCOL_TEST_ENABLE_DATA            (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_ZERO_LENGTH
#define BL_PROTOCOL_TEST_ENABLE_ZERO_LENGTH     (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_MAX_LENGTH
#define BL_PROTOCOL_TEST_ENABLE_MAX_LENGTH      (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_ALL_COMMANDS
#define BL_PROTOCOL_TEST_ENABLE_ALL_COMMANDS    (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_INVALID_COMMAND
#define BL_PROTOCOL_TEST_ENABLE_INVALID_COMMAND (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_EXTRACT_COMMAND
#define BL_PROTOCOL_TEST_ENABLE_EXTRACT_COMMAND (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_CRC
#define BL_PROTOCOL_TEST_ENABLE_CRC             (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_INVALID_SOF
#define BL_PROTOCOL_TEST_ENABLE_INVALID_SOF     (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_INVALID_LENGTH
#define BL_PROTOCOL_TEST_ENABLE_INVALID_LENGTH (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_SHORT_FRAME
#define BL_PROTOCOL_TEST_ENABLE_SHORT_FRAME     (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_TRUNCATED
#define BL_PROTOCOL_TEST_ENABLE_TRUNCATED       (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_SMALL_BUFFER
#define BL_PROTOCOL_TEST_ENABLE_SMALL_BUFFER    (1U)
#endif

#ifndef BL_PROTOCOL_TEST_ENABLE_NULL_PARAMETER
#define BL_PROTOCOL_TEST_ENABLE_NULL_PARAMETER  (1U)
#endif


/* ============================================================================
 * Public Functions
 * ========================================================================== */

void BL_Protocol_Test(void);


#ifdef __cplusplus
}
#endif

#endif /* BL_PROTOCOL_TEST_H */