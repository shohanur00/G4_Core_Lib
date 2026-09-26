#include "ring_buffer.h"
#include <stddef.h>


/*==========================================================
 * Initialization
 *==========================================================*/

void RingBuffer_Setup(RingBuffer_t *rb,
                      uint8_t *buffer,
                      uint32_t size)
{
    rb->buffer    = buffer;
    rb->readIndex = 0U;
    rb->writeIndex = 0U;
    rb->mask      = size - 1U;
}


void RingBuffer_Reset(RingBuffer_t *rb)
{
    rb->readIndex  = 0U;
    rb->writeIndex = 0U;
}


/*==========================================================
 * Status / Information
 *==========================================================*/

bool RingBuffer_Empty(const RingBuffer_t *rb)
{
    return (rb->readIndex == rb->writeIndex);
}


bool RingBuffer_Full(const RingBuffer_t *rb)
{
    uint32_t nextWriteIndex;

    nextWriteIndex = (rb->writeIndex + 1U) & rb->mask;

    return (nextWriteIndex == rb->readIndex);
}


uint32_t RingBuffer_Capacity(const RingBuffer_t *rb)
{
    /*
     * One slot is reserved to distinguish
     * between FULL and EMPTY states.
     *
     * Actual buffer size = mask + 1
     * Usable capacity    = mask
     */
    return rb->mask;
}


uint32_t RingBuffer_Count(const RingBuffer_t *rb)
{
    return (rb->writeIndex - rb->readIndex) & rb->mask;
}


uint32_t RingBuffer_Free(const RingBuffer_t *rb)
{
    return RingBuffer_Capacity(rb) - RingBuffer_Count(rb);
}


/*==========================================================
 * Single Item Operations
 *==========================================================*/

bool RingBuffer_Write(RingBuffer_t *rb,
                      uint8_t byte)
{
    uint32_t writeIndex;
    uint32_t nextWriteIndex;

    writeIndex = rb->writeIndex;

    nextWriteIndex = (writeIndex + 1U) & rb->mask;

    /* Buffer full */
    if (nextWriteIndex == rb->readIndex)
    {
        return false;
    }

    rb->buffer[writeIndex] = byte;

    rb->writeIndex = nextWriteIndex;

    return true;
}


bool RingBuffer_Read(RingBuffer_t *rb,
                     uint8_t *byte)
{
    uint32_t readIndex;

    readIndex = rb->readIndex;

    /* Buffer empty */
    if (readIndex == rb->writeIndex)
    {
        return false;
    }

    *byte = rb->buffer[readIndex];

    rb->readIndex = (readIndex + 1U) & rb->mask;

    return true;
}


bool RingBuffer_Peek(const RingBuffer_t *rb,
                     uint8_t *byte)
{
    uint32_t readIndex;

    readIndex = rb->readIndex;

    /* Buffer empty */
    if (readIndex == rb->writeIndex)
    {
        return false;
    }

    *byte = rb->buffer[readIndex];

    /*
     * readIndex is NOT changed.
     */

    return true;
}


bool RingBuffer_Discard(RingBuffer_t *rb)
{
    /* Buffer empty */
    if (rb->readIndex == rb->writeIndex)
    {
        return false;
    }

    rb->readIndex = (rb->readIndex + 1U) & rb->mask;

    return true;
}


/*==========================================================
 * Bulk Operations
 *==========================================================*/

uint32_t RingBuffer_WriteBuffer(RingBuffer_t *rb,
                                const uint8_t *data,
                                uint32_t length)
{
    uint32_t written = 0U;

    while (written < length)
    {
        if (!RingBuffer_Write(rb, data[written]))
        {
            break;
        }

        written++;
    }

    return written;
}


uint32_t RingBuffer_ReadBuffer(RingBuffer_t *rb,
                               uint8_t *data,
                               uint32_t length)
{
    uint32_t read = 0U;

    while (read < length)
    {
        if (!RingBuffer_Read(rb, &data[read]))
        {
            break;
        }

        read++;
    }

    return read;
}


uint32_t RingBuffer_DiscardBuffer(RingBuffer_t *rb,
                                  uint32_t length)
{
    uint32_t discarded = 0U;

    while (discarded < length)
    {
        if (!RingBuffer_Discard(rb))
        {
            break;
        }

        discarded++;
    }

    return discarded;
}


/*==========================================================
 * Zero-Copy / Direct Access
 *==========================================================*/

const uint8_t *RingBuffer_PeekBuffer(const RingBuffer_t *rb,
                                     uint32_t *length)
{
    uint32_t readIndex;
    uint32_t writeIndex;
    uint32_t count;
    uint32_t contiguousLength;
    uint32_t bufferSize;

    readIndex  = rb->readIndex;
    writeIndex = rb->writeIndex;

    /* Buffer empty */
    if (readIndex == writeIndex)
    {
        *length = 0U;
        return NULL;
    }

    count = RingBuffer_Count(rb);

    /*
     * Actual buffer size = mask + 1
     */
    bufferSize = rb->mask + 1U;

    /*
     * Number of bytes available before reaching
     * the physical end of the buffer.
     */
    contiguousLength = bufferSize - readIndex;

    if (contiguousLength > count)
    {
        contiguousLength = count;
    }

    *length = contiguousLength;

    return &rb->buffer[readIndex];
}


/*==========================================================
 * Search
 *==========================================================*/

bool RingBuffer_Find(const RingBuffer_t *rb,
                     uint8_t value,
                     uint32_t *offset)
{
    uint32_t index;
    uint32_t count;
    uint32_t i;

    count = RingBuffer_Count(rb);
    index = rb->readIndex;

    for (i = 0U; i < count; i++)
    {
        if (rb->buffer[index] == value)
        {
            *offset = i;
            return true;
        }

        index = (index + 1U) & rb->mask;
    }

    return false;
}