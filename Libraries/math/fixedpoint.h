#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>

/**
 * @file fixed_point.h
 * @brief Generic fixed-point arithmetic utilities.
 *
 * Hardware-independent fixed-point utilities for embedded systems.
 *
 * The fixed-point format is defined by the number of fractional bits.
 *
 * Example:
 *     fractional_bits = 15  -> Q15
 *     fractional_bits = 8   -> Q8
 *     fractional_bits = 16  -> Q16
 */


/* -------------------------------------------------------------------------- */
/* Conversion                                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Converts an integer value to fixed-point representation.
 *
 * @param value           Input integer value.
 * @param fractional_bits Number of fractional bits.
 *
 * @return Fixed-point representation.
 */
int32_t FixedPointFromInt_i32(int32_t value,
                              uint8_t fractional_bits);


/**
 * @brief Converts a fixed-point value to an integer.
 *
 * Fractional information is discarded.
 *
 * @param value           Fixed-point value.
 * @param fractional_bits Number of fractional bits.
 *
 * @return Integer value.
 */
int32_t FixedPointToInt_i32(int32_t value,
                            uint8_t fractional_bits);


/* -------------------------------------------------------------------------- */
/* Arithmetic                                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Adds two fixed-point values.
 *
 * Both values must use the same number of fractional bits.
 *
 * @param a First fixed-point value.
 * @param b Second fixed-point value.
 *
 * @return Fixed-point sum.
 */
int32_t FixedPointAdd_i32(int32_t a,
                          int32_t b);


/**
 * @brief Subtracts two fixed-point values.
 *
 * Both values must use the same number of fractional bits.
 *
 * @param a First fixed-point value.
 * @param b Second fixed-point value.
 *
 * @return Fixed-point difference.
 */
int32_t FixedPointSub_i32(int32_t a,
                          int32_t b);


/**
 * @brief Multiplies two fixed-point values.
 *
 * Both values must use the same number of fractional bits.
 *
 * @param a              First fixed-point value.
 * @param b              Second fixed-point value.
 * @param fractional_bits Number of fractional bits.
 *
 * @return Fixed-point product.
 */
int32_t FixedPointMul_i32(int32_t a,
                          int32_t b,
                          uint8_t fractional_bits);


/**
 * @brief Divides two fixed-point values.
 *
 * Both values must use the same number of fractional bits.
 *
 * @param a              Dividend.
 * @param b              Divisor.
 * @param fractional_bits Number of fractional bits.
 *
 * @return Fixed-point quotient.
 *
 * @note Requires b != 0.
 */
int32_t FixedPointDiv_i32(int32_t a,
                          int32_t b,
                          uint8_t fractional_bits);


/* -------------------------------------------------------------------------- */
/* Scaling                                                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Changes the number of fractional bits of a fixed-point value.
 *
 * Converts a fixed-point value from one Q-format to another.
 *
 * @param value           Input fixed-point value.
 * @param from_fractional_bits Current number of fractional bits.
 * @param to_fractional_bits   Desired number of fractional bits.
 *
 * @return Converted fixed-point value.
 */
int32_t FixedPointRescale_i32(int32_t value,
                              uint8_t from_fractional_bits,
                              uint8_t to_fractional_bits);


/**
 * @brief Shifts a fixed-point value left.
 *
 * @param value Fixed-point value.
 * @param shift Number of bits to shift.
 *
 * @return Shifted value.
 */
int32_t FixedPointShiftLeft_i32(int32_t value,
                                uint8_t shift);


/**
 * @brief Shifts a fixed-point value right.
 *
 * @param value Fixed-point value.
 * @param shift Number of bits to shift.
 *
 * @return Shifted value.
 */
int32_t FixedPointShiftRight_i32(int32_t value,
                                 uint8_t shift);


/* -------------------------------------------------------------------------- */
/* Saturation                                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Saturates a fixed-point value to a specified range.
 *
 * @param value Input fixed-point value.
 * @param min   Minimum allowed value.
 * @param max   Maximum allowed value.
 *
 * @return Saturated fixed-point value.
 */
int32_t FixedPointSaturate_i32(int32_t value,
                               int32_t min,
                               int32_t max);


/**
 * @brief Checks whether a fixed-point value is within a range.
 *
 * @param value Input fixed-point value.
 * @param min   Minimum allowed value.
 * @param max   Maximum allowed value.
 *
 * @return 1 if value is within the range, otherwise 0.
 */
uint8_t FixedPointInRange_i32(int32_t value,
                              int32_t min,
                              int32_t max);


#endif /* FIXED_POINT_H */

