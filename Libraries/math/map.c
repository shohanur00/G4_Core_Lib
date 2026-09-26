
#include "map.h"

int32_t Map_i32(int32_t value,
                int32_t in_min,
                int32_t in_max,
                int32_t out_min,
                int32_t out_max)
{
    return ((value - in_min) * (out_max - out_min))
           / (in_max - in_min)
           + out_min;
}

uint32_t Map_u32(uint32_t value,
                 uint32_t in_min,
                 uint32_t in_max,
                 uint32_t out_min,
                 uint32_t out_max)
{
    return ((value - in_min) * (out_max - out_min))
           / (in_max - in_min)
           + out_min;
}
