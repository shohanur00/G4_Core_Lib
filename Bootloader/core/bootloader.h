#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "protocol/bl_protocol.h"


/* --------------------------------------------------------------------------
 * Bootloader State
 * -------------------------------------------------------------------------- */

/**
 * @brief Bootloader state machine states.
 */
typedef enum
{
    BL_STATE_WAIT_SYNC = 0,
    BL_STATE_CONNECTED,
    BL_STATE_WAIT_FW_LENGTH,
    BL_STATE_WAIT_FW_START_ADDRESS,
    BL_STATE_READY,
    BL_STATE_RECEIVING,
    BL_STATE_COMPLETE,
    BL_STATE_PROGRAMMING,
    BL_STATE_VALID

} BL_State_t;


/* --------------------------------------------------------------------------
 * Bootloader Error Codes
 * -------------------------------------------------------------------------- */

/**
 * @brief Error codes reported by the bootloader.
 */
typedef enum
{
    BL_ERROR_NONE            = 0x00U,
    BL_ERROR_CRC             = 0x01U,
    BL_ERROR_LENGTH          = 0x02U,
    BL_ERROR_COMMAND         = 0x03U,
    BL_ERROR_STATE           = 0x04U,
    BL_ERROR_ADDRESS         = 0x05U,
    BL_ERROR_SIZE            = 0x06U,
    BL_ERROR_PROTOCOL        = 0x07U,
    BL_ERROR_DEVICE_ID       = 0x08U,
    BL_ERROR_NO_RETRY_PACKET = 0x09U,
    BL_ERROR_FLASH_ERASE     = 0x0AU,
    BL_ERROR_DATA            = 0x0BU,


} BL_ErrorCode_t;


/* --------------------------------------------------------------------------
 * Firmware Information
 * -------------------------------------------------------------------------- */

/**
 * @brief Stores firmware update information.
 */
typedef struct
{
    uint32_t size;
    uint32_t offset;
    uint32_t crc;
    uint32_t start_address;
    uint32_t write_address;
    uint32_t received_size;

} BL_FirmwareInfo_t;


/* --------------------------------------------------------------------------
 * Bootloader Public API
 * -------------------------------------------------------------------------- */

/**
 * @brief Initialize the bootloader.
 */
void Bootloader_Init(void);


/**
 * @brief Process incoming bootloader data.
 *
 * Call periodically from the main loop.
 */
void Bootloader_Process(void);

BL_State_t Bootloader_GetState(void);

#ifdef __cplusplus
}
#endif

#endif /* BOOTLOADER_H */