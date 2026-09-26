#include "bit.h"


/* --------------------------------------------------------------------------
 * Bit Manipulation
 * Generic, hardware-independent bit-level utility functions.
 * -------------------------------------------------------------------------- */


uint32_t bit_set(uint32_t value, uint8_t position)
{
    return value | (1UL << position);
}


uint32_t bit_clear(uint32_t value, uint8_t position)
{
    return value & ~(1UL << position);
}


uint32_t bit_toggle(uint32_t value, uint8_t position)
{
    return value ^ (1UL << position);
}


bool bit_get(uint32_t value, uint8_t position)
{
    return ((value >> position) & 1UL) != 0U;
}


uint32_t bits_write(uint32_t value,
                    uint32_t field,
                    uint8_t position,
                    uint8_t width)
{
    uint32_t mask;

    if ((width == 0U) || (position >= 32U))
    {
        return value;
    }

    if (width >= 32U - position)
    {
        mask = 0xFFFFFFFFUL << position;
    }
    else
    {
        mask = ((1UL << width) - 1UL) << position;
    }

    value &= ~mask;
    value |= (field << position) & mask;

    return value;
}


uint32_t bits_extract(uint32_t value,
                      uint8_t position,
                      uint8_t width)
{
    uint32_t mask;

    if ((width == 0U) || (position >= 32U))
    {
        return 0U;
    }

    if (width >= 32U - position)
    {
        mask = 0xFFFFFFFFUL >> position;
    }
    else
    {
        mask = (1UL << width) - 1UL;
    }

    return (value >> position) & mask;
}


uint32_t bits_insert(uint32_t value,
                     uint32_t field,
                     uint8_t position,
                     uint8_t width)
{
    return bits_write(value, field, position, width);
}


uint8_t bit_count(uint32_t value)
{
    uint8_t count = 0U;

    while (value != 0U)
    {
        value &= value - 1U;
        count++;
    }

    return count;
}


int8_t bit_find_first_set(uint32_t value)
{
    int8_t position = 0;

    if (value == 0U)
    {
        return -1;
    }

    while ((value & 1U) == 0U)
    {
        value >>= 1U;
        position++;
    }

    return position;
}


uint32_t bits_reverse(uint32_t value)
{
    uint32_t reversed = 0U;
    uint8_t i;

    for (i = 0U; i < 32U; i++)
    {
        reversed <<= 1U;
        reversed |= value & 1U;
        value >>= 1U;
    }

    return reversed;
}


uint32_t bits_rotate_left(uint32_t value, uint8_t count)
{
    count %= 32U;

    if (count == 0U)
    {
        return value;
    }

    return (value << count) | (value >> (32U - count));
}


uint32_t bits_rotate_right(uint32_t value, uint8_t count)
{
    count %= 32U;

    if (count == 0U)
    {
        return value;
    }

    return (value >> count) | (value << (32U - count));
}