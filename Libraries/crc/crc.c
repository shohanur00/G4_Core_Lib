#include "crc.h"
#include <stddef.h>


/* ============================================================================
 * CRC-8
 * ========================================================================== */

uint8_t CRC8(
    const uint8_t *data,
    uint32_t        length
)
{
    uint8_t crc;
    uint32_t i;
    uint8_t bit;

    if (data == NULL)
    {
        return 0U;
    }

    crc = CRC8_INITIAL;

    for (i = 0U; i < length; i++)
    {
        crc ^= data[i];

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x80U) != 0U)
            {
                crc = (uint8_t)(
                    (crc << 1U) ^
                    CRC8_POLYNOMIAL
                );
            }
            else
            {
                crc <<= 1U;
            }
        }
    }

    return crc ^ CRC8_XOR_OUT;
}


/* ============================================================================
 * CRC-8/SAE-J1850
 * ========================================================================== */

uint8_t CRC8_SAE_J1850(
    const uint8_t *data,
    uint32_t        length
)
{
    uint8_t crc;
    uint32_t i;
    uint8_t bit;

    if (data == NULL)
    {
        return 0U;
    }

    crc = CRC8_SAE_J1850_INITIAL;

    for (i = 0U; i < length; i++)
    {
        crc ^= data[i];

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x80U) != 0U)
            {
                crc = (uint8_t)(
                    (crc << 1U) ^
                    CRC8_SAE_J1850_POLYNOMIAL
                );
            }
            else
            {
                crc <<= 1U;
            }
        }
    }

    return crc ^ CRC8_SAE_J1850_XOR_OUT;
}


/* ============================================================================
 * CRC-16/CCITT-FALSE
 * ========================================================================== */

uint16_t CRC16_CCITT_FALSE(
    const uint8_t *data,
    uint32_t        length
)
{
    uint16_t crc;
    uint32_t i;
    uint8_t bit;

    if (data == NULL)
    {
        return 0U;
    }

    crc = CRC16_CCITT_FALSE_INITIAL;

    for (i = 0U; i < length; i++)
    {
        crc ^= (uint16_t)data[i] << 8U;

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x8000U) != 0U)
            {
                crc = (uint16_t)(
                    (crc << 1U) ^
                    CRC16_CCITT_FALSE_POLYNOMIAL
                );
            }
            else
            {
                crc <<= 1U;
            }
        }
    }

    return crc ^ CRC16_CCITT_FALSE_XOR_OUT;
}


/* ============================================================================
 * CRC-16/IBM / ARC
 *
 * Reflected algorithm.
 * ========================================================================== */

uint16_t CRC16_IBM(
    const uint8_t *data,
    uint32_t        length
)
{
    uint16_t crc;
    uint32_t i;
    uint8_t bit;

    if (data == NULL)
    {
        return 0U;
    }

    crc = CRC16_IBM_INITIAL;

    for (i = 0U; i < length; i++)
    {
        crc ^= data[i];

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x0001U) != 0U)
            {
                crc = (uint16_t)(
                    (crc >> 1U) ^
                    0xA001U
                );
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc ^ CRC16_IBM_XOR_OUT;
}


/* ============================================================================
 * CRC-16/MODBUS
 *
 * Reflected algorithm.
 * ========================================================================== */

uint16_t CRC16_MODBUS(
    const uint8_t *data,
    uint32_t        length
)
{
    uint16_t crc;
    uint32_t i;
    uint8_t bit;

    if (data == NULL)
    {
        return 0U;
    }

    crc = CRC16_MODBUS_INITIAL;

    for (i = 0U; i < length; i++)
    {
        crc ^= data[i];

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x0001U) != 0U)
            {
                crc = (uint16_t)(
                    (crc >> 1U) ^
                    0xA001U
                );
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc ^ CRC16_MODBUS_XOR_OUT;
}


/* ============================================================================
 * USB CRC-5
 *
 * USB Token CRC.
 *
 * USB CRC-5 is calculated LSB-first.
 * ========================================================================== */

uint8_t CRC5_USB(
    uint16_t data,
    uint8_t  bit_count
)
{
    uint8_t crc;
    uint8_t i;

    if (bit_count == 0U)
    {
        return CRC5_USB_XOR_OUT;
    }

    if (bit_count > 16U)
    {
        bit_count = 16U;
    }

    crc = CRC5_USB_INITIAL;

    for (i = 0U; i < bit_count; i++)
    {
        uint8_t data_bit;
        uint8_t crc_bit;

        data_bit = (uint8_t)(data & 0x01U);
        crc_bit  = (uint8_t)(crc & 0x01U);

        data >>= 1U;

        if ((data_bit ^ crc_bit) != 0U)
        {
            crc = (uint8_t)(
                (crc >> 1U) ^
                0x14U
            );
        }
        else
        {
            crc >>= 1U;
        }

        crc &= 0x1FU;
    }

    return (uint8_t)(
        crc ^ CRC5_USB_XOR_OUT
    );
}


/* ============================================================================
 * USB CRC-16
 *
 * USB DATA packet CRC.
 *
 * USB CRC-16 is transmitted LSB-first.
 * ========================================================================== */

uint16_t CRC16_USB(
    const uint8_t *data,
    uint32_t        length
)
{
    uint16_t crc;
    uint32_t i;
    uint8_t bit;

    if (data == NULL)
    {
        return 0U;
    }

    crc = CRC16_USB_INITIAL;

    for (i = 0U; i < length; i++)
    {
        crc ^= data[i];

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x0001U) != 0U)
            {
                crc = (uint16_t)(
                    (crc >> 1U) ^
                    0x8408U
                );
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc ^ CRC16_USB_XOR_OUT;
}


/* ============================================================================
 * CRC-32/IEEE
 *
 * Reflected algorithm.
 * ========================================================================== */

uint32_t CRC32_IEEE(
    const uint8_t *data,
    uint32_t        length
)
{
    uint32_t crc;
    uint32_t i;
    uint8_t bit;

    if (data == NULL)
    {
        return 0U;
    }

    crc = CRC32_IEEE_INITIAL;

    for (i = 0U; i < length; i++)
    {
        crc ^= data[i];

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x00000001UL) != 0U)
            {
                crc = (crc >> 1U) ^ 0xEDB88320UL;
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc ^ CRC32_IEEE_XOR_OUT;
}