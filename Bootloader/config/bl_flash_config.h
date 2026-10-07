#ifndef BL_FLASH_CONFIG_H
#define BL_FLASH_CONFIG_H


/* ============================================================================
 * Flash Memory Configuration
 * ========================================================================== */

#define BL_FLASH_START_ADDRESS          (0x08000000UL)
#define BL_FLASH_SIZE                   (128UL * 1024UL)
#define BL_FLASH_PAGE_SIZE              (2UL * 1024UL)

#define BL_FLASH_END_ADDRESS            \
    (BL_FLASH_START_ADDRESS + BL_FLASH_SIZE - 1UL)


/* ============================================================================
 * Bootloader Memory
 * ========================================================================== */

#define BL_BOOTLOADER_START_ADDRESS     (0x08000000UL)
#define BL_BOOTLOADER_SIZE              (16UL * 1024UL)
#define BL_BOOTLOADER_END_ADDRESS       \
    (BL_BOOTLOADER_START_ADDRESS + BL_BOOTLOADER_SIZE - 1UL)

/* ============================================================================
 * Main Application Memory
 * ========================================================================== */

#define BL_APP_START_ADDRESS            (0x08004000UL)
#define BL_APP_SIZE                     (108UL * 1024UL)
#define BL_APP_END_ADDRESS              \
    (BL_APP_START_ADDRESS + BL_APP_SIZE - 1UL)

/* ============================================================================
 * Bootloader Metadata
 * ========================================================================== */

#define BL_METADATA_START_ADDRESS       (0x0801F000UL)
#define BL_METADATA_SIZE                (2UL * 1024UL)
#define BL_METADATA_END_ADDRESS         \
    (BL_METADATA_START_ADDRESS + BL_METADATA_SIZE - 1UL)

/* ============================================================================
 * Main Application Data
 * ========================================================================== */

#define BL_APP_DATA_START_ADDRESS       (0x0801F800UL)
#define BL_APP_DATA_SIZE                (2UL * 1024UL)
#define BL_APP_DATA_END_ADDRESS         \
    (BL_APP_DATA_START_ADDRESS + BL_APP_DATA_SIZE - 1UL)



/* --------------------------------------------------------------------------
 * FLASH CRC
 * -------------------------------------------------------------------------- */


#define BL_FLASH_CRC16_POLYNOMIAL    (0x1021U)
#define BL_FLASH_CRC16_INITIAL       (0xFFFFU)


#endif /* BL_FLASH_CONFIG_H */