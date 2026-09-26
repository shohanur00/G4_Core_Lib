#ifndef MAP_H
#define MAP_H

#include <stdint.h>

/**
 * @brief Map a signed 32-bit value from one range to another.
 *
 * @param value   Input value.
 * @param in_min  Input range minimum.
 * @param in_max  Input range maximum.
 * @param out_min Output range minimum.
 * @param out_max Output range maximum.
 *
 * @return Mapped signed 32-bit value.
 *
 * @note Requires in_max != in_min.
 */
int32_t Map_i32(int32_t value,
                int32_t in_min,
                int32_t in_max,
                int32_t out_min,
                int32_t out_max);

/**
 * @brief Map an unsigned 32-bit value from one range to another.
 *
 * @param value   Input value.
 * @param in_min  Input range minimum.
 * @param in_max  Input range maximum.
 * @param out_min Output range minimum.
 * @param out_max Output range maximum.
 *
 * @return Mapped unsigned 32-bit value.
 *
 * @note Requires in_max != in_min.
 */
uint32_t Map_u32(uint32_t value,
                 uint32_t in_min,
                 uint32_t in_max,
                 uint32_t out_min,
                 uint32_t out_max);

#endif /* MAP_H */

