#ifndef CRC_H
#define CRC_H

#include <stdint.h>

/* ============================================================================
 * CRC-8
 * ========================================================================== */

#define CRC8_POLYNOMIAL             (0x07U)
#define CRC8_INITIAL                (0x00U)
#define CRC8_XOR_OUT                (0x00U)

uint8_t CRC8(
    const uint8_t *data,
    uint32_t        length
);


/* ============================================================================
 * CRC-8/SAE-J1850
 * ========================================================================== */

#define CRC8_SAE_J1850_POLYNOMIAL   (0x1DU)
#define CRC8_SAE_J1850_INITIAL      (0xFFU)
#define CRC8_SAE_J1850_XOR_OUT      (0xFFU)

uint8_t CRC8_SAE_J1850(
    const uint8_t *data,
    uint32_t        length
);


/* ============================================================================
 * CRC-16/CCITT-FALSE
 * ========================================================================== */

#define CRC16_CCITT_FALSE_POLYNOMIAL    (0x1021U)
#define CRC16_CCITT_FALSE_INITIAL       (0xFFFFU)
#define CRC16_CCITT_FALSE_XOR_OUT       (0x0000U)

uint16_t CRC16_CCITT_FALSE(
    const uint8_t *data,
    uint32_t        length
);


/* ============================================================================
 * CRC-16/IBM / ARC
 * ========================================================================== */

#define CRC16_IBM_POLYNOMIAL        (0x8005U)
#define CRC16_IBM_INITIAL           (0x0000U)
#define CRC16_IBM_XOR_OUT           (0x0000U)

uint16_t CRC16_IBM(
    const uint8_t *data,
    uint32_t        length
);


/* ============================================================================
 * CRC-16/MODBUS
 * ========================================================================== */

#define CRC16_MODBUS_POLYNOMIAL     (0x8005U)
#define CRC16_MODBUS_INITIAL        (0xFFFFU)
#define CRC16_MODBUS_XOR_OUT        (0x0000U)

uint16_t CRC16_MODBUS(
    const uint8_t *data,
    uint32_t        length
);


/* ============================================================================
 * USB CRC-5
 *
 * Used by USB Token packets.
 *
 * Polynomial:
 *     x^5 + x^2 + 1
 *
 * Polynomial:
 *     0x05
 * ========================================================================== */

#define CRC5_USB_POLYNOMIAL         (0x05U)
#define CRC5_USB_INITIAL            (0x1FU)
#define CRC5_USB_XOR_OUT            (0x1FU)

uint8_t CRC5_USB(
    uint16_t data,
    uint8_t  bit_count
);


/* ============================================================================
 * USB CRC-16
 *
 * Used by USB Data packets.
 *
 * Polynomial:
 *     x^16 + x^15 + x^2 + 1
 *
 * Polynomial:
 *     0x8005
 * ========================================================================== */

#define CRC16_USB_POLYNOMIAL        (0x8005U)
#define CRC16_USB_INITIAL           (0xFFFFU)
#define CRC16_USB_XOR_OUT           (0xFFFFU)

uint16_t CRC16_USB(
    const uint8_t *data,
    uint32_t        length
);


/* ============================================================================
 * CRC-32/IEEE
 * ========================================================================== */

#define CRC32_IEEE_POLYNOMIAL       (0x04C11DB7UL)
#define CRC32_IEEE_INITIAL          (0xFFFFFFFFUL)
#define CRC32_IEEE_XOR_OUT          (0xFFFFFFFFUL)

uint32_t CRC32_IEEE(
    const uint8_t *data,
    uint32_t        length
);

#endif /* CRC_H */