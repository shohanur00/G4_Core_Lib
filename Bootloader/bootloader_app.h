#ifndef BOOTLOADER_APP_H
#define BOOTLOADER_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------
 * Bootloader Application
 * -------------------------------------------------------------------------- */

void Bootloader_App_Deinit(void);
void Bootloader_App_Setup(void);
void Bootloader_App_Loop(void);

#ifdef __cplusplus
}
#endif

#endif /* BOOTLOADER_APP_H */