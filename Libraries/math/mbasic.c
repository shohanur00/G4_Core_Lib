#include "mbasic.h"

#include <limits.h>


uint32_t MbasicAbs_i32(int32_t value)
{
    if (value < 0)
    {
        return (uint32_t)(-(value + 1)) + 1U;
    }

    return (uint32_t)value;
}


void MbasicSwap_i32(int32_t *a, int32_t *b)
{
    int32_t temp = *a;

    *a = *b;
    *b = temp;
}


int32_t MbasicClamp_i32(int32_t value,
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


int32_t MbasicSatAdd_i32(int32_t a, int32_t b)
{
    if (b > 0 && a > (INT32_MAX - b))
    {
        return INT32_MAX;
    }

    if (b < 0 && a < (INT32_MIN - b))
    {
        return INT32_MIN;
    }

    return a + b;
}


int32_t MbasicSatSub_i32(int32_t a, int32_t b)
{
    if (b < 0 && a > (INT32_MAX + b))
    {
        return INT32_MAX;
    }

    if (b > 0 && a < (INT32_MIN + b))
    {
        return INT32_MIN;
    }

    return a - b;
}


uint32_t MbasicDivCeil_u32(uint32_t value,
                           uint32_t divisor)
{
    return (value + divisor - 1U) / divisor;
}


uint32_t MbasicRoundUp_u32(uint32_t value,
                           uint32_t multiple)
{
    return ((value + multiple - 1U) / multiple) * multiple;
}


uint32_t MbasicAlignUp_u32(uint32_t value,
                           uint32_t alignment)
{
    return ((value + alignment - 1U) / alignment) * alignment;
}


uint32_t MbasicGcd_u32(uint32_t a,
                       uint32_t b)
{
    while (b != 0U)
    {
        uint32_t remainder = a % b;

        a = b;
        b = remainder;
    }

    return a;
}


uint32_t MbasicLcm_u32(uint32_t a,
                       uint32_t b)
{
    if (a == 0U || b == 0U)
    {
        return 0U;
    }

    return (a / MbasicGcd_u32(a, b)) * b;
}


bool MbasicIsPowerOfTwo_u32(uint32_t value)
{
    return (value != 0U) &&
           ((value & (value - 1U)) == 0U);
}
