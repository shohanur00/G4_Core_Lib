#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------
 * Firmware
 * -------------------------------------------------------------------------- */

void Firmware_Setup(void);
void Firmware_Loop(void);

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */