#ifndef BL_TRANSPORT_H
#define BL_TRANSPORT_H

#include <stdint.h>
#include <stdbool.h>


/* ============================================================================
 * Transport API
 * ========================================================================== */

/**
 * @brief Initialize the bootloader transport.
 */
void BL_Transport_Init(void);


/**
 * @brief Get the number of bytes available for reading.
 *
 * @return Number of bytes available.
 */
uint16_t BL_Transport_DataAvailable(void);


/**
 * @brief Read one byte from the transport.
 *
 * @param[out] byte Pointer to store the received byte.
 *
 * @return true  if a byte was read successfully.
 * @return false if no byte is available or an error occurred.
 */
bool BL_Transport_ReadByte(
    uint8_t *byte
);


/**
 * @brief Write data through the bootloader transport.
 *
 * @param[in] data   Pointer to data buffer.
 * @param[in] length Number of bytes to transmit.
 *
 * @return true  if the data was accepted successfully.
 * @return false if transmission could not be started/completed.
 */
bool BL_Transport_Write(
    const uint8_t *data,
    uint16_t length
);


#endif /* BL_TRANSPORT_H */