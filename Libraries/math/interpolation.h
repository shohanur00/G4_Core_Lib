#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <stdint.h>

/**
 * @file interpolation.h
 * @brief Interpolation utilities for embedded systems.
 *
 * Hardware-independent integer-based interpolation utilities
 * commonly used in embedded firmware.
 */


/**
 * @brief Performs linear interpolation between two points.
 *
 * Calculates the Y value corresponding to X using two known
 * points (x0, y0) and (x1, y1).
 *
 * @param x  Input X value.
 * @param x0 X value of the first point.
 * @param y0 Y value of the first point.
 * @param x1 X value of the second point.
 * @param y1 Y value of the second point.
 *
 * @return Interpolated Y value as a signed 32-bit integer.
 *
 * @note Requires x0 != x1.
 */
int32_t InterpolationLinear_i32(int32_t x,
                                int32_t x0,
                                int32_t y0,
                                int32_t x1,
                                int32_t y1);


/**
 * @brief Performs piecewise linear interpolation.
 *
 * Finds the interval containing the input X value from a
 * lookup table and performs linear interpolation between
 * the two adjacent points.
 *
 * @param x       Input X value.
 * @param x_table Sorted X-axis lookup table.
 * @param y_table Corresponding Y-axis lookup table.
 * @param size    Number of points in the lookup table.
 *
 * @return Interpolated Y value as a signed 32-bit integer.
 *
 * @note x_table must be sorted in ascending order.
 * @note Requires size >= 2.
 */
int32_t InterpolationPiecewiseLinear_i32(int32_t x,
                                         const int32_t *x_table,
                                         const int32_t *y_table,
                                         uint32_t size);


/**
 * @brief Performs polynomial interpolation.
 *
 * Estimates the Y value at the specified X value using
 * multiple known X/Y data points.
 *
 * @param x       Input X value.
 * @param x_table X-axis data points.
 * @param y_table Corresponding Y-axis data points.
 * @param size    Number of data points.
 *
 * @return Interpolated Y value as a signed 32-bit integer.
 *
 * @note Requires size >= 2.
 * @note All X values must be unique.
 */
int32_t InterpolationPolynomial_i32(int32_t x,
                                    const int32_t *x_table,
                                    const int32_t *y_table,
                                    uint32_t size);


#endif /* INTERPOLATION_H */

