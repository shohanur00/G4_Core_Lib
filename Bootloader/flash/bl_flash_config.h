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
 * Application Memory
 * ========================================================================== */

#define BL_APP_START_ADDRESS            (0x08004000UL)
#define BL_APP_SIZE                     (108UL * 1024UL)

#define BL_APP_END_ADDRESS              (BL_APP_START_ADDRESS + BL_APP_SIZE - 1UL)


/* ============================================================================
 * Data Memory
 * ========================================================================== */

#define BL_DATA_START_ADDRESS           (0x0801F000UL)
#define BL_DATA_SIZE                    (4UL * 1024UL)

#define BL_DATA_END_ADDRESS             (BL_DATA_START_ADDRESS + BL_DATA_SIZE - 1UL)


#endif /* BL_FLASH_CONFIG_H */