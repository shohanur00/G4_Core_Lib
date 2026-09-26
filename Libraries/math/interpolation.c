#include "interpolation.h"

#include <stddef.h>


int32_t InterpolationLinear_i32(int32_t x,
                                int32_t x0,
                                int32_t y0,
                                int32_t x1,
                                int32_t y1)
{
    return y0 + (((x - x0) * (y1 - y0)) / (x1 - x0));
}


int32_t InterpolationPiecewiseLinear_i32(int32_t x,
                                         const int32_t *x_table,
                                         const int32_t *y_table,
                                         uint32_t size)
{
    uint32_t i;

    if ((x_table == NULL) ||
        (y_table == NULL) ||
        (size < 2U))
    {
        return 0;
    }

    if (x <= x_table[0])
    {
        return y_table[0];
    }

    if (x >= x_table[size - 1U])
    {
        return y_table[size - 1U];
    }

    for (i = 0U; i < (size - 1U); i++)
    {
        if (x <= x_table[i + 1U])
        {
            return InterpolationLinear_i32(
                x,
                x_table[i],
                y_table[i],
                x_table[i + 1U],
                y_table[i + 1U]
            );
        }
    }

    return y_table[size - 1U];
}


int32_t InterpolationPolynomial_i32(int32_t x,
                                    const int32_t *x_table,
                                    const int32_t *y_table,
                                    uint32_t size)
{
    int64_t result = 0;
    uint32_t i;
    uint32_t j;

    if ((x_table == NULL) ||
        (y_table == NULL) ||
        (size < 2U))
    {
        return 0;
    }

    for (i = 0U; i < size; i++)
    {
        int64_t term = y_table[i];

        for (j = 0U; j < size; j++)
        {
            if (i != j)
            {
                term = (term * (x - x_table[j]))
                       / (x_table[i] - x_table[j]);
            }
        }

        result += term;
    }

    return (int32_t)result;
}