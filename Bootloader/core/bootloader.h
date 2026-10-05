
#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------
 * Bootloader Public API
 * -------------------------------------------------------------------------- */

/**
 * @brief Initialize the bootloader protocol parser and transport.
 */
void Bootloader_Init(void);


/**
 * @brief Process incoming transport data and handle complete packets.
 *
 * Call this function periodically from the main loop.
 */
void Bootloader_Process(void);

#ifdef __cplusplus
}
#endif

#endif /* BOOTLOADER_H */