#ifndef BL_FLASH_H
#define BL_FLASH_H


/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>
#include <stdbool.h>

#include "../config/bl_flash_config.h"


/* ============================================================================
 * Initialization
 * ========================================================================== */

/**
 * @brief Initialize the Flash module.
 *
 * @return true  Flash module initialized successfully.
 * @return false Flash module initialization failed.
 */
bool BL_Flash_Init(void);


/* ============================================================================
 * Erase
 * ========================================================================== */

/**
 * @brief Erase a region of the application Flash.
 *
 * The requested address range must be completely inside the configured
 * application Flash region.
 *
 * @param address Start address of the Flash region.
 * @param length  Number of bytes to erase.
 *
 * @return true  Flash erase completed successfully.
 * @return false Invalid range or erase operation failed.
 */
bool BL_Flash_Erase(
    uint32_t address,
    uint32_t length
);


/**
 * @brief Erase a region of the metadata Flash.
 *
 * The requested address range must be completely inside the configured
 * metadata Flash region.
 *
 * @param address Start address of the metadata region.
 * @param length  Number of bytes to erase.
 *
 * @return true  Metadata erase completed successfully.
 * @return false Invalid range or erase operation failed.
 */
bool BL_Flash_Erase_MetaData(
    uint32_t address,
    uint32_t length
);


/* ============================================================================
 * Write
 * ========================================================================== */

/**
 * @brief Write data to the application Flash.
 *
 * @param address Start address where the data will be written.
 * @param data    Pointer to the source data buffer.
 * @param length  Number of bytes to write.
 *
 * @return true  Data written successfully.
 * @return false Invalid parameters, invalid range, or write failure.
 */
bool BL_Flash_Write(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
);


/**
 * @brief Write data to the metadata Flash.
 *
 * @param address Start address where the data will be written.
 * @param data    Pointer to the source data buffer.
 * @param length  Number of bytes to write.
 *
 * @return true  Metadata written successfully.
 * @return false Invalid parameters, invalid range, or write failure.
 */
bool BL_Flash_Write_MetaData(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
);


/* ============================================================================
 * Verify
 * ========================================================================== */

/**
 * @brief Verify application Flash contents against a data buffer.
 *
 * @param address Start address of the Flash region.
 * @param data    Pointer to the expected data.
 * @param length  Number of bytes to verify.
 *
 * @return true  Flash contents match the expected data.
 * @return false Verification failed or the requested range is invalid.
 */
bool BL_Flash_Verify(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
);


/**
 * @brief Verify metadata Flash contents against a data buffer.
 *
 * @param address Start address of the metadata region.
 * @param data    Pointer to the expected data.
 * @param length  Number of bytes to verify.
 *
 * @return true  Metadata contents match the expected data.
 * @return false Verification failed or the requested range is invalid.
 */
bool BL_Flash_Verify_MetaData(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
);


/* ============================================================================
 * Erased Check
 * ========================================================================== */

/**
 * @brief Check whether an application Flash region is completely erased.
 *
 * The STM32 Flash erased state is expected to be 0xFF.
 *
 * @param address Start address of the Flash region.
 * @param length  Number of bytes to check.
 *
 * @return true  All bytes in the region are 0xFF.
 * @return false Region contains programmed data or the range is invalid.
 */
bool BL_Flash_IsErased(
    uint32_t address,
    uint32_t length
);


/**
 * @brief Check whether a metadata Flash region is completely erased.
 *
 * The STM32 Flash erased state is expected to be 0xFF.
 *
 * @param address Start address of the metadata region.
 * @param length  Number of bytes to check.
 *
 * @return true  All bytes in the region are 0xFF.
 * @return false Region contains programmed data or the range is invalid.
 */
bool BL_Flash_IsErased_MetaData(
    uint32_t address,
    uint32_t length
);


/* ============================================================================
 * Read
 * ========================================================================== */

/**
 * @brief Read data from the application Flash.
 *
 * @param address Start address of the Flash region.
 * @param data    Pointer to the destination buffer.
 * @param length  Number of bytes to read.
 *
 * @return true  Data read successfully.
 * @return false Invalid parameters or the requested range is invalid.
 */
bool BL_Flash_Read(
    uint32_t address,
    uint8_t *data,
    uint32_t length
);


/**
 * @brief Read data from the metadata Flash.
 *
 * @param address Start address of the metadata region.
 * @param data    Pointer to the destination buffer.
 * @param length  Number of bytes to read.
 *
 * @return true  Data read successfully.
 * @return false Invalid parameters or the requested range is invalid.
 */
bool BL_Flash_Read_MetaData(
    uint32_t address,
    uint8_t *data,
    uint32_t length
);


/* ============================================================================
 * CRC
 * ========================================================================== */

/**
 * @brief Calculate CRC16 over an application Flash region.
 *
 * The CRC algorithm and initial value are defined in the Flash
 * configuration.
 *
 * @param address Start address of the Flash region.
 * @param length  Number of bytes included in the CRC calculation.
 *
 * @return Calculated CRC16 value.
 *
 * @note Returns 0U if the requested Flash range is invalid.
 */
uint16_t BL_Flash_CalculateCRC(
    uint32_t address,
    uint32_t length
);


#endif /* BL_FLASH_H */
