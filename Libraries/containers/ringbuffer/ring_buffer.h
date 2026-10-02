#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>


/*==========================================================
 * Ring Buffer Object
 *==========================================================*/

typedef struct
{
    volatile uint8_t  *buffer;

    uint32_t readIndex;
    uint32_t writeIndex;

    uint32_t mask;

} RingBuffer_t;


/*==========================================================
 * Initialization
 *==========================================================*/

/**
 * @brief Initialize a ring buffer.
 *
 * @param rb      Pointer to ring buffer object.
 * @param buffer  User-provided storage buffer.
 * @param size    Buffer size.
 *
 * @note Size must be a power of 2 and >= 2.
 *       One slot is reserved to distinguish full from empty.
 */
void RingBuffer_Setup(RingBuffer_t *rb,
                      volatile uint8_t *buffer,
                      uint32_t size);


/**
 * @brief Reset the ring buffer.
 *
 * @note All stored data will be discarded.
 */
void RingBuffer_Reset(RingBuffer_t *rb);


/*==========================================================
 * Status / Information
 *==========================================================*/

/**
 * @brief Check whether the ring buffer is empty.
 *
 * @return true  if empty.
 * @return false otherwise.
 */
bool RingBuffer_Empty(const RingBuffer_t *rb);


/**
 * @brief Check whether the ring buffer is full.
 *
 * @return true  if full.
 * @return false otherwise.
 */
bool RingBuffer_Full(const RingBuffer_t *rb);


/**
 * @brief Get the total usable capacity of the ring buffer.
 *
 * @return Maximum number of items that can be stored.
 *
 * @note For a buffer size of 8, capacity is 7.
 */
uint32_t RingBuffer_Capacity(const RingBuffer_t *rb);


/**
 * @brief Get the number of stored items.
 *
 * @return Number of available items.
 */
uint32_t RingBuffer_Count(const RingBuffer_t *rb);


/**
 * @brief Get the number of free slots.
 *
 * @return Number of available free slots.
 */
uint32_t RingBuffer_Free(const RingBuffer_t *rb);


/*==========================================================
 * Single Item Operations
 *==========================================================*/

/**
 * @brief Write one byte into the ring buffer.
 *
 * @param rb    Pointer to ring buffer object.
 * @param byte  Byte to write.
 *
 * @return true  if successful.
 * @return false if buffer is full.
 */
bool RingBuffer_Write(RingBuffer_t *rb,
                      uint8_t byte);


/**
 * @brief Read one byte from the ring buffer.
 *
 * @param rb    Pointer to ring buffer object.
 * @param byte  Pointer to store the received byte.
 *
 * @return true  if successful.
 * @return false if buffer is empty.
 */
bool RingBuffer_Read(RingBuffer_t *rb,
                     uint8_t *byte);


/**
 * @brief Read the next byte without removing it.
 *
 * @param rb    Pointer to ring buffer object.
 * @param byte  Pointer to store the peeked byte.
 *
 * @return true  if data is available.
 * @return false if buffer is empty.
 */
bool RingBuffer_Peek(const RingBuffer_t *rb,
                     uint8_t *byte);


/**
 * @brief Discard the oldest byte.
 *
 * @param rb  Pointer to ring buffer object.
 *
 * @return true  if a byte was discarded.
 * @return false if buffer is empty.
 */
bool RingBuffer_Discard(RingBuffer_t *rb);


/*==========================================================
 * Bulk Operations
 *==========================================================*/

/**
 * @brief Write multiple bytes into the ring buffer.
 *
 * @param rb      Pointer to ring buffer object.
 * @param data    Source data buffer.
 * @param length  Number of bytes to write.
 *
 * @return Number of bytes actually written.
 */
uint32_t RingBuffer_WriteBuffer(RingBuffer_t *rb,
                                const uint8_t *data,
                                uint32_t length);


/**
 * @brief Read multiple bytes from the ring buffer.
 *
 * @param rb      Pointer to ring buffer object.
 * @param data    Destination buffer.
 * @param length  Maximum number of bytes to read.
 *
 * @return Number of bytes actually read.
 */
uint32_t RingBuffer_ReadBuffer(RingBuffer_t *rb,
                               uint8_t *data,
                               uint32_t length);


/**
 * @brief Discard multiple bytes from the ring buffer.
 *
 * @param rb      Pointer to ring buffer object.
 * @param length  Number of bytes to discard.
 *
 * @return Number of bytes actually discarded.
 */
uint32_t RingBuffer_DiscardBuffer(RingBuffer_t *rb,
                                  uint32_t length);


/*==========================================================
 * Zero-Copy / Direct Access
 *==========================================================*/

/**
 * @brief Get a pointer to the first contiguous readable block.
 *
 * @param rb      Pointer to ring buffer object.
 * @param length  Pointer to receive the block length.
 *
 * @return Pointer to readable data.
 *         NULL if buffer is empty.
 *
 * @note The returned pointer is valid only until the ring
 *       buffer state is modified.
 */
const uint8_t *RingBuffer_PeekBuffer(const RingBuffer_t *rb,
                                     uint32_t *length);


/*==========================================================
 * Search
 *==========================================================*/

/**
 * @brief Find a byte in the ring buffer.
 *
 * @param rb      Pointer to ring buffer object.
 * @param value   Byte value to search for.
 * @param offset  Pointer to receive offset from readIndex.
 *
 * @return true  if value is found.
 * @return false otherwise.
 */
bool RingBuffer_Find(const RingBuffer_t *rb,
                     uint8_t value,
                     uint32_t *offset);


#endif /* RING_BUFFER_H */