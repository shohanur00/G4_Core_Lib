#ifndef BL_FLASH_H
#define BL_FLASH_H


/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>
#include <stdbool.h>

#include "../config/bl_flash_config.h"


/* ============================================================================
 * Flash Initialization
 * ========================================================================== */

/**
 * @brief Initialize the bootloader flash module.
 *
 * @return true  Flash module initialized successfully.
 * @return false Flash module initialization failed.
 */
bool BL_Flash_Init(void);


/* ============================================================================
 * Flash Erase
 * ========================================================================== */

/**
 * @brief Erase a specified flash memory region.
 *
 * The address and length must fall within the configured application
 * flash region.
 *
 * @param address Start address of the flash region.
 * @param length  Number of bytes to erase.
 *
 * @return true  Flash erase completed successfully.
 * @return false Flash erase failed or the requested range is invalid.
 */
bool BL_Flash_Erase(
    uint32_t address,
    uint32_t length
);


/* ============================================================================
 * Flash Write
 * ========================================================================== */

/**
 * @brief Write data to flash memory.
 *
 * @param address Start address where the data will be written.
 * @param data    Pointer to the source data buffer.
 * @param length  Number of bytes to write.
 *
 * @return true  Data written successfully.
 * @return false Flash write failed or the requested range is invalid.
 */
bool BL_Flash_Write(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
);


/* ============================================================================
 * Flash Verify
 * ========================================================================== */

/**
 * @brief Verify flash contents against a data buffer.
 *
 * Reads the specified flash region and compares it with the provided
 * data buffer.
 *
 * @param address Start address of the flash region.
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


/* ============================================================================
 * Flash Erased Check
 * ========================================================================== */

/**
 * @brief Check whether a flash memory region is erased.
 *
 * The function checks whether all bytes in the specified region contain
 * the configured erased flash value.
 *
 * @param address Start address of the flash region.
 * @param length  Number of bytes to check.
 *
 * @return true  The complete region is erased.
 * @return false The region contains programmed data or the range is invalid.
 */
bool BL_Flash_IsErased(
    uint32_t address,
    uint32_t length
);


/* ============================================================================
 * Flash Read
 * ========================================================================== */

/**
 * @brief Read data from flash memory.
 *
 * @param address Start address of the flash region.
 * @param data    Pointer to the destination buffer.
 * @param length  Number of bytes to read.
 *
 * @return true  Data read successfully.
 * @return false Flash read failed or the requested range is invalid.
 */
bool BL_Flash_Read(
    uint32_t address,
    uint8_t *data,
    uint32_t length
);


/* ============================================================================
 * Flash CRC Calculation
 * ========================================================================== */

/**
 * @brief Calculate CRC16 over a flash memory region.
 *
 * The CRC is calculated over the specified flash address range using
 * the configured CRC algorithm.
 *
 * @param address Start address of the flash region.
 * @param length  Number of bytes included in the CRC calculation.
 *
 * @return Calculated CRC16 value.
 */
uint16_t BL_Flash_CalculateCRC(
    uint32_t address,
    uint32_t length
);


#endif /* BL_FLASH_H */
