#ifndef BL_METADATA_H
#define BL_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Firmware Metadata
 * ========================================================================== */

/**
 * @brief Persistent firmware information stored in flash.
 *
 * The metadata is used by the bootloader to identify and validate
 * the installed main application after reset.
 */
typedef struct
{
    uint32_t magic;
    uint32_t start_address;
    uint32_t size;
    uint16_t crc;
    uint16_t version;
    uint32_t update_status;
    uint32_t reserved[3];

} BL_FirmwareMetadata_t;

/* ============================================================================
 * Metadata API
 * ========================================================================== */

/**
 * @brief Save firmware metadata to flash.
 *
 * @param[in] metadata Pointer to firmware metadata.
 *
 * @return true  Metadata saved successfully.
 * @return false Metadata save failed.
 */
bool BL_Metadata_Save(
    const BL_FirmwareMetadata_t *metadata
);

/**
 * @brief Read firmware metadata from flash.
 *
 * @param[out] metadata Pointer to metadata structure.
 *
 * @return true  Metadata read successfully.
 * @return false Metadata read failed.
 */
bool BL_Metadata_Read(
    BL_FirmwareMetadata_t *metadata
);

/**
 * @brief Validate firmware metadata.
 *
 * @param[in] metadata Pointer to firmware metadata.
 *
 * @return true  Metadata is valid.
 * @return false Metadata is invalid.
 */
bool BL_Metadata_IsValid(
    const BL_FirmwareMetadata_t *metadata
);

#ifdef __cplusplus
}
#endif

#endif /* BL_METADATA_H */