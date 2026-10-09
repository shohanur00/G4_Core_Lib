#ifndef BOOTLOADER_H
#define BOOTLOADER_H


#ifdef __cplusplus
extern "C" {
#endif


/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>
#include <stdbool.h>

#include "protocol/bl_protocol.h"


/* ============================================================================
 * Bootloader State
 * ========================================================================== */

/**
 * @brief Bootloader state machine states.
 *
 * Defines the current operating state of the bootloader during the
 * firmware update process.
 */
typedef enum
{
    BL_STATE_WAIT_SYNC = 0,       /**< Waiting for PC synchronization. */
    BL_STATE_CONNECTED,           /**< PC connection established. */
    BL_STATE_WAIT_FW_LENGTH,      /**< Waiting for firmware size. */
    BL_STATE_WAIT_FW_START_ADDRESS, /**< Waiting for application start address. */
    BL_STATE_READY,               /**< Ready to receive firmware data. */
    BL_STATE_RECEIVING,           /**< Receiving firmware data. */
    BL_STATE_COMPLETE,            /**< Firmware data reception completed. */
    BL_STATE_PROGRAMMING,         /**< Programming firmware to flash. */
    BL_STATE_VALID,                /**< Firmware successfully validated. */
    BL_STATE_WAIT_DEVICE_ID_CONFIRM,
    BL_STATE_WAIT_DEVICE_ID_REQ,

} BL_State_t;


/* ============================================================================
 * Bootloader Error Codes
 * ========================================================================== */

/**
 * @brief Error codes reported by the bootloader.
 *
 * These error codes are used in NACK responses to indicate the reason
 * for a rejected or failed bootloader operation.
 */
typedef enum
{
    BL_ERROR_NONE            = 0x00U, /**< No error. */
    BL_ERROR_CRC             = 0x01U, /**< CRC verification failed. */
    BL_ERROR_LENGTH          = 0x02U, /**< Invalid packet/data length. */
    BL_ERROR_COMMAND         = 0x03U, /**< Invalid or unsupported command. */
    BL_ERROR_STATE           = 0x04U, /**< Command received in invalid state. */
    BL_ERROR_ADDRESS         = 0x05U, /**< Invalid flash/application address. */
    BL_ERROR_SIZE            = 0x06U, /**< Invalid firmware size. */
    BL_ERROR_PROTOCOL        = 0x07U, /**< Protocol format error. */
    BL_ERROR_DEVICE_ID       = 0x08U, /**< Device ID error. */
    BL_ERROR_NO_RETRY_PACKET = 0x09U, /**< No packet available for retry. */
    BL_ERROR_FLASH_ERASE     = 0x0AU, /**< Flash erase operation failed. */
    BL_ERROR_DATA            = 0x0BU  /**< Invalid firmware data. */

} BL_ErrorCode_t;


/* ============================================================================
 * Firmware Information
 * ========================================================================== */

/**
 * @brief Stores firmware update information.
 *
 * Contains the firmware size, address information, CRC, and data reception
 * status required during the firmware update process.
 */
typedef struct
{
    uint32_t size;           /**< Firmware image size in bytes. */
    uint32_t offset;         /**< Offset from the application base address. */
    uint32_t crc;            /**< Firmware CRC value. */
    uint32_t start_address;  /**< Application start address. */
    uint32_t write_address;  /**< Current flash write address. */
    uint32_t received_size;  /**< Number of firmware bytes received. */

} BL_FirmwareInfo_t;


/* ============================================================================
 * Bootloader Public API
 * ========================================================================== */

/**
 * @brief Initialize the bootloader.
 *
 * Initializes the bootloader state and required internal resources.
 */
void Bootloader_Init(void);


/**
 * @brief Process incoming bootloader communication.
 *
 * Checks for and processes received protocol packets. This function should
 * be called periodically from the main application loop.
 */
void Bootloader_Process(void);


/**
 * @brief Get the current bootloader state.
 *
 * @return Current bootloader state.
 */
BL_State_t Bootloader_GetState(void);


/**
 * @brief Validate the existing application firmware.
 *
 * Checks whether a valid application is available for execution.
 *
 * @return true  Application firmware is valid.
 * @return false Application firmware is invalid or unavailable.
 */
bool Bootloader_ValidateApplication(void);


/**
 * @brief Return the bootloader to the idle state.
 *
 * Resets the bootloader state to wait for a new PC synchronization request.
 */
void Bootloader_GetBackTo_Idle(void);


#ifdef __cplusplus
}
#endif


#endif /* BOOTLOADER_H */
