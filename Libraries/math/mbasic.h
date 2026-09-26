#ifndef MBASIC_H
#define MBASIC_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @file mbasic.h
 * @brief Basic mathematical utilities for embedded systems.
 *
 * Hardware-independent integer-based mathematical utilities
 * commonly used in embedded firmware.
 */


/**
 * @brief Returns the absolute value of a signed 32-bit integer.
 *
 * @param value Input signed integer.
 * @return Absolute value as an unsigned 32-bit integer.
 */
uint32_t MbasicAbs_i32(int32_t value);


/**
 * @brief Swaps two signed 32-bit integer values.
 *
 * @param a Pointer to the first value.
 * @param b Pointer to the second value.
 */
void MbasicSwap_i32(int32_t *a, int32_t *b);


/**
 * @brief Limits a value to a specified range.
 *
 * @param value Input value.
 * @param min Minimum allowed value.
 * @param max Maximum allowed value.
 * @return Value limited to the range [min, max].
 */
int32_t MbasicClamp_i32(int32_t value,
                        int32_t min,
                        int32_t max);


/**
 * @brief Adds two signed 32-bit integers with saturation.
 *
 * @param a First operand.
 * @param b Second operand.
 * @return Saturated sum.
 */
int32_t MbasicSatAdd_i32(int32_t a,
                         int32_t b);


/**
 * @brief Subtracts two signed 32-bit integers with saturation.
 *
 * @param a First operand.
 * @param b Second operand.
 * @return Saturated difference.
 */
int32_t MbasicSatSub_i32(int32_t a,
                         int32_t b);


/**
 * @brief Performs unsigned integer division rounded upward.
 *
 * @param value Dividend.
 * @param divisor Divisor. Must be greater than zero.
 * @return Ceiling of value / divisor.
 */
uint32_t MbasicDivCeil_u32(uint32_t value,
                           uint32_t divisor);


/**
 * @brief Rounds a value upward to the next multiple.
 *
 * @param value Input value.
 * @param multiple Positive rounding multiple.
 * @return Rounded-up value.
 */
uint32_t MbasicRoundUp_u32(uint32_t value,
                           uint32_t multiple);


/**
 * @brief Aligns a value upward to an alignment boundary.
 *
 * Useful for memory, buffer, DMA, and protocol alignment.
 *
 * @param value Input value.
 * @param alignment Alignment boundary.
 * @return Aligned value.
 */
uint32_t MbasicAlignUp_u32(uint32_t value,
                           uint32_t alignment);


/**
 * @brief Calculates the greatest common divisor (GCD).
 *
 * @param a First unsigned integer.
 * @param b Second unsigned integer.
 * @return Greatest common divisor.
 */
uint32_t MbasicGcd_u32(uint32_t a,
                       uint32_t b);


/**
 * @brief Calculates the least common multiple (LCM).
 *
 * @param a First unsigned integer.
 * @param b Second unsigned integer.
 * @return Least common multiple.
 */
uint32_t MbasicLcm_u32(uint32_t a,
                       uint32_t b);


/**
 * @brief Checks whether a value is a power of two.
 *
 * @param value Input unsigned integer.
 * @return true if value is a power of two, otherwise false.
 */
bool MbasicIsPowerOfTwo_u32(uint32_t value);


#endif /* MBASIC_H */
