#ifndef BIT_H
#define BIT_H

#include <stdint.h>
#include <stdbool.h>

/* --------------------------------------------------------------------------
 * Bit Manipulation
 * Generic, hardware-independent bit-level utility functions.
 * -------------------------------------------------------------------------- */

/**
 * @brief Set a specific bit to 1.
 *
 * @param value     Input value.
 * @param position  Bit position (0 = LSB).
 *
 * @return Value with the selected bit set.
 */
uint32_t bit_set(uint32_t value, uint8_t position);

/**
 * @brief Clear a specific bit to 0.
 *
 * @param value     Input value.
 * @param position  Bit position (0 = LSB).
 *
 * @return Value with the selected bit cleared.
 */
uint32_t bit_clear(uint32_t value, uint8_t position);

/**
 * @brief Toggle a specific bit.
 *
 * @param value     Input value.
 * @param position  Bit position (0 = LSB).
 *
 * @return Value with the selected bit toggled.
 */
uint32_t bit_toggle(uint32_t value, uint8_t position);

/**
 * @brief Read the state of a specific bit.
 *
 * @param value     Input value.
 * @param position  Bit position (0 = LSB).
 *
 * @return true if the bit is set, otherwise false.
 */
bool bit_get(uint32_t value, uint8_t position);

/**
 * @brief Write a value into a consecutive bit field.
 *
 * @param value     Original value.
 * @param field     Value to write.
 * @param position  Starting bit position.
 * @param width     Number of bits in the field.
 *
 * @return Value with the selected bit field updated.
 */
uint32_t bits_write(uint32_t value,
                    uint32_t field,
                    uint8_t position,
                    uint8_t width);

/**
 * @brief Extract a consecutive bit field.
 *
 * @param value     Input value.
 * @param position  Starting bit position.
 * @param width     Number of bits to extract.
 *
 * @return Extracted bit field, right-aligned.
 */
uint32_t bits_extract(uint32_t value,
                      uint8_t position,
                      uint8_t width);

/**
 * @brief Insert a value into a consecutive bit field.
 *
 * @param value     Original value.
 * @param field     Value to insert.
 * @param position  Starting bit position.
 * @param width     Number of bits in the field.
 *
 * @return Value with the selected bit field replaced.
 */
uint32_t bits_insert(uint32_t value,
                     uint32_t field,
                     uint8_t position,
                     uint8_t width);

/**
 * @brief Count the number of set bits (1s).
 *
 * @param value  Input value.
 *
 * @return Number of set bits.
 */
uint8_t bit_count(uint32_t value);

/**
 * @brief Find the position of the first set bit.
 *
 * Searches from the least significant bit (LSB).
 *
 * @param value  Input value.
 *
 * @return Bit position, or -1 if no bit is set.
 */
int8_t bit_find_first_set(uint32_t value);

/**
 * @brief Reverse the bit order of a 32-bit value.
 *
 * @param value  Input value.
 *
 * @return Bit-reversed value.
 */
uint32_t bits_reverse(uint32_t value);

/**
 * @brief Rotate bits to the left.
 *
 * @param value  Input value.
 * @param count  Number of bit positions to rotate.
 *
 * @return Rotated value.
 */
uint32_t bits_rotate_left(uint32_t value, uint8_t count);

/**
 * @brief Rotate bits to the right.
 *
 * @param value  Input value.
 * @param count  Number of bit positions to rotate.
 *
 * @return Rotated value.
 */
uint32_t bits_rotate_right(uint32_t value, uint8_t count);

#endif /*BIT_H */