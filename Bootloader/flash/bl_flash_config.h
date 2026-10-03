#ifndef BL_FLASH_CONFIG_H
#define BL_FLASH_CONFIG_H


/* ============================================================================
 * Flash Memory Configuration
 * ========================================================================== */

#define BL_FLASH_START_ADDRESS          (0x08000000UL)
#define BL_FLASH_SIZE                   (128UL * 1024UL)


/* ============================================================================
 * Bootloader Memory
 * ========================================================================== */

#define BL_BOOTLOADER_START_ADDRESS     (0x08000000UL)
#define BL_BOOTLOADER_SIZE              (16UL * 1024UL)


/* ============================================================================
 * Application Memory
 * ========================================================================== */

#define BL_APP_START_ADDRESS            (0x08004000UL)
#define BL_APP_SIZE                     (112UL * 1024UL)


#endif /* BL_FLASH_CONFIG_H */