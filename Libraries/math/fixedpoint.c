#include "fixedpoint.h"

#include <limits.h>


/* -------------------------------------------------------------------------- */
/* Conversion                                                                  */
/* -------------------------------------------------------------------------- */

int32_t FixedPointFromInt_i32(int32_t value,
                              uint8_t fractional_bits)
{
    if (fractional_bits >= 31U)
    {
        return 0;
    }

    return (int32_t)((int64_t)value << fractional_bits);
}


int32_t FixedPointToInt_i32(int32_t value,
                            uint8_t fractional_bits)
{
    if (fractional_bits >= 31U)
    {
        return 0;
    }

    return value >> fractional_bits;
}


/* -------------------------------------------------------------------------- */
/* Arithmetic                                                                  */
/* -------------------------------------------------------------------------- */

int32_t FixedPointAdd_i32(int32_t a,
                          int32_t b)
{
    return a + b;
}


int32_t FixedPointSub_i32(int32_t a,
                          int32_t b)
{
    return a - b;
}


int32_t FixedPointMul_i32(int32_t a,
                          int32_t b,
                          uint8_t fractional_bits)
{
    int64_t result;

    if (fractional_bits >= 31U)
    {
        return 0;
    }

    result = ((int64_t)a * (int64_t)b) >> fractional_bits;

    if (result > INT32_MAX)
    {
        return INT32_MAX;
    }

    if (result < INT32_MIN)
    {
        return INT32_MIN;
    }

    return (int32_t)result;
}


int32_t FixedPointDiv_i32(int32_t a,
                          int32_t b,
                          uint8_t fractional_bits)
{
    int64_t numerator;
    int64_t result;

    if ((b == 0) || (fractional_bits >= 31U))
    {
        return 0;
    }

    numerator = (int64_t)a << fractional_bits;
    result = numerator / b;

    if (result > INT32_MAX)
    {
        return INT32_MAX;
    }

    if (result < INT32_MIN)
    {
        return INT32_MIN;
    }

    return (int32_t)result;
}


/* -------------------------------------------------------------------------- */
/* Scaling                                                                     */
/* -------------------------------------------------------------------------- */

int32_t FixedPointRescale_i32(int32_t value,
                              uint8_t from_fractional_bits,
                              uint8_t to_fractional_bits)
{
    uint8_t shift;

    if ((from_fractional_bits >= 31U) ||
        (to_fractional_bits >= 31U))
    {
        return 0;
    }

    if (from_fractional_bits == to_fractional_bits)
    {
        return value;
    }

    if (to_fractional_bits > from_fractional_bits)
    {
        shift = to_fractional_bits - from_fractional_bits;

        if (shift >= 31U)
        {
            return 0;
        }

        return (int32_t)((int64_t)value << shift);
    }

    shift = from_fractional_bits - to_fractional_bits;

    return value >> shift;
}


int32_t FixedPointShiftLeft_i32(int32_t value,
                                uint8_t shift)
{
    int64_t result;

    if (shift >= 31U)
    {
        return 0;
    }

    result = (int64_t)value << shift;

    if (result > INT32_MAX)
    {
        return INT32_MAX;
    }

    if (result < INT32_MIN)
    {
        return INT32_MIN;
    }

    return (int32_t)result;
}


int32_t FixedPointShiftRight_i32(int32_t value,
                                 uint8_t shift)
{
    if (shift >= 31U)
    {
        return (value < 0) ? -1 : 0;
    }

    return value >> shift;
}


/* -------------------------------------------------------------------------- */
/* Saturation                                                                  */
/* -------------------------------------------------------------------------- */

int32_t FixedPointSaturate_i32(int32_t value,
                               int32_t min,
                               int32_t max)
{
    if (value < min)
    {
        return min;
    }

    if (value > max)
    {
        return max;
    }

    return value;
}


uint8_t FixedPointInRange_i32(int32_t value,
                              int32_t min,
                              int32_t max)
{
    return (value >= min && value <= max) ? 1U : 0U;
}
