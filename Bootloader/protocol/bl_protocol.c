#include "bl_protocol.h"

#include "crc/crc.h"
#include "cdefs/cdefs.h"

#include <stdint.h>
#include <stdbool.h>


/* ============================================================================
 * Private Functions
 * ========================================================================== */

static uint16_t BL_Protocol_ComputeCRC(
    const BL_Protocol_Packet_t *packet
)
{
    uint8_t crc_data[BL_PROTOCOL_MAX_DATA_SIZE + 1U];

    if (packet == NULL)
    {
        return 0U;
    }

    /*
     * CRC covers:
     *
     * [LENGTH] [DATA...]
     *
     * SOF is not included.
     */

    crc_data[0] = packet->length;

    for (uint8_t i = 0U; i < packet->length; i++)
    {
        crc_data[1U + i] = packet->data[i];
    }

    return CRC16_CCITT_FALSE(
        crc_data,
        (uint32_t)packet->length + 1U
    );
}


/* ============================================================================
 * Packet To Bytes
 * ========================================================================== */

bool BL_Protocol_PacketToBytes(
    const BL_Protocol_Packet_t *packet,
    uint8_t                    *buffer,
    uint16_t                    buffer_size,
    uint16_t                   *length
)
{
    uint16_t frame_length;

    if ((packet == NULL) ||
        (buffer == NULL) ||
        (length == NULL))
    {
        return false;
    }

    if (packet->length > BL_PROTOCOL_MAX_DATA_SIZE)
    {
        return false;
    }

    /*
     * Frame:
     *
     * [SOF] [LENGTH] [DATA...] [CRC_H] [CRC_L]
     *
     * Total = 1 + 1 + DATA + 2
     *       = 4 + DATA length
     */

    frame_length = BL_PROTOCOL_OVERHEAD_SIZE + packet->length;

    if (buffer_size < frame_length)
    {
        return false;
    }

    buffer[0] = packet->sof;
    buffer[1] = packet->length;

    for (uint8_t i = 0U; i < packet->length; i++)
    {
        buffer[2U + i] = packet->data[i];
    }

    /*
     * CRC is transmitted MSB first.
     */

    buffer[2U + packet->length] =
        (uint8_t)(packet->crc >> 8U);

    buffer[3U + packet->length] =
        (uint8_t)(packet->crc & 0xFFU);

    *length = frame_length;

    return true;
}


/* ============================================================================
 * Bytes To Packet
 * ========================================================================== */

bool BL_Protocol_BytesToPacket(
    const uint8_t        *buffer,
    uint16_t              buffer_length,
    BL_Protocol_Packet_t *packet
)
{
    uint16_t frame_length;
    uint16_t received_crc;
    uint16_t calculated_crc;

    if ((buffer == NULL) ||
        (packet == NULL))
    {
        return false;
    }

    /*
     * Minimum frame:
     *
     * [SOF] [LENGTH] [CRC_H] [CRC_L]
     */

    if (buffer_length < BL_PROTOCOL_MIN_FRAME_SIZE)
    {
        return false;
    }

    /* Check SOF */

    if (buffer[0] != BL_PROTOCOL_SOF)
    {
        return false;
    }

    /* Check payload length */

    if (buffer[1] > BL_PROTOCOL_MAX_DATA_SIZE)
    {
        return false;
    }

    /*
     * Calculate expected frame length.
     */

    frame_length =
        BL_PROTOCOL_OVERHEAD_SIZE + buffer[1];

    if (buffer_length < frame_length)
    {
        return false;
    }

    packet->sof    = buffer[0];
    packet->length = buffer[1];

    for (uint8_t i = 0U; i < packet->length; i++)
    {
        packet->data[i] = buffer[2U + i];
    }

    /*
     * CRC is received MSB first.
     */

    received_crc =
        ((uint16_t)buffer[2U + packet->length] << 8U) |
        (uint16_t)buffer[3U + packet->length];

    packet->crc = received_crc;

    /*
     * Verify CRC.
     */

    calculated_crc = BL_Protocol_ComputeCRC(packet);

    if (received_crc != calculated_crc)
    {
        return false;
    }

    return true;
}


/* ============================================================================
 * Command Check
 * ========================================================================== */

bool BL_Protocol_IsCommand(
    const BL_Protocol_Packet_t *packet,
    BL_Command_t                command
)
{
    if (packet == NULL)
    {
        return false;
    }

    if (packet->length == 0U)
    {
        return false;
    }

    return (packet->data[0] == (uint8_t)command);
}


/* ============================================================================
 * Create Command Packet
 * ========================================================================== */

void BL_Protocol_CreateCommandPacket(
    BL_Protocol_Packet_t *packet,
    BL_Command_t          command
)
{
    if (packet == NULL)
    {
        return;
    }

    packet->sof    = BL_PROTOCOL_SOF;
    packet->length = 1U;

    packet->data[0] = (uint8_t)command;

    packet->crc = BL_Protocol_ComputeCRC(packet);
}


/* ============================================================================
 * Create Data Packet
 * ========================================================================== */

void BL_Protocol_CreateDataPacket(
    BL_Protocol_Packet_t *packet,
    const uint8_t        *data,
    uint8_t               length
)
{
    if (packet == NULL)
    {
        return;
    }

    if ((data == NULL) && (length > 0U))
    {
        return;
    }

    if (length > BL_PROTOCOL_MAX_DATA_SIZE)
    {
        return;
    }

    packet->sof    = BL_PROTOCOL_SOF;
    packet->length = length;

    for (uint8_t i = 0U; i < length; i++)
    {
        packet->data[i] = data[i];
    }

    packet->crc = BL_Protocol_ComputeCRC(packet);
}
