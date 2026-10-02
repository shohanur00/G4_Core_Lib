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
    uint8_t crc_data[BL_PROTOCOL_MAX_LENGTH];

    if (packet == NULL)
    {
        return 0U;
    }

    /*
     * CRC covers:
     *
     * [LENGTH] [TYPE] [COMMAND] [DATA...]
     *
     * SOF is not included.
     */

    crc_data[0] = packet->length;
    crc_data[1] = packet->type;
    crc_data[2] = packet->command;

    for (uint8_t i = 0U; i < packet->length -
                             BL_PROTOCOL_FIXED_DATA_SIZE; i++)
    {
        crc_data[3U + i] = packet->data[i];
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
    uint8_t  data_length;

    if ((packet == NULL) ||
        (buffer == NULL) ||
        (length == NULL))
    {
        return false;
    }

    /*
     * LENGTH represents:
     *
     * TYPE + COMMAND + DATA
     */

    if (packet->length < BL_PROTOCOL_MIN_LENGTH)
    {
        return false;
    }

    if (packet->length > BL_PROTOCOL_MAX_LENGTH)
    {
        return false;
    }

    data_length =
        (uint8_t)(packet->length -
                  BL_PROTOCOL_FIXED_DATA_SIZE);

    /*
     * Frame:
     *
     * [SOF] [LENGTH] [TYPE] [COMMAND] [DATA...] [CRC_H] [CRC_L]
     *
     * Total = 4 + LENGTH
     */

    frame_length =
        BL_PROTOCOL_OVERHEAD_SIZE + packet->length;

    if (buffer_size < frame_length)
    {
        return false;
    }

    buffer[0] = packet->sof;
    buffer[1] = packet->length;

    buffer[2] = packet->type;
    buffer[3] = packet->command;

    for (uint8_t i = 0U; i < data_length; i++)
    {
        buffer[4U + i] = packet->data[i];
    }

    /*
     * CRC is transmitted MSB first.
     */

    buffer[4U + data_length] =
        (uint8_t)(packet->crc >> 8U);

    buffer[5U + data_length] =
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
    uint8_t  data_length;

    if ((buffer == NULL) ||
        (packet == NULL))
    {
        return false;
    }

    /*
     * Minimum frame:
     *
     * [SOF] [LENGTH] [TYPE] [COMMAND] [CRC_H] [CRC_L]
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

    /* Check LENGTH */

    if (buffer[1] < BL_PROTOCOL_MIN_LENGTH)
    {
        return false;
    }

    if (buffer[1] > BL_PROTOCOL_MAX_LENGTH)
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

    /*
     * Extract TYPE and COMMAND.
     */

    packet->type    = buffer[2];
    packet->command = buffer[3];

    data_length =
        (uint8_t)(packet->length -
                  BL_PROTOCOL_FIXED_DATA_SIZE);

    /*
     * Extract DATA.
     */

    for (uint8_t i = 0U; i < data_length; i++)
    {
        packet->data[i] = buffer[4U + i];
    }

    /*
     * CRC is received MSB first.
     */

    received_crc =
        ((uint16_t)buffer[4U + data_length] << 8U) |
        (uint16_t)buffer[5U + data_length];

    packet->crc = received_crc;

    /*
     * Verify CRC.
     */

    calculated_crc =
        BL_Protocol_ComputeCRC(packet);

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

    if (packet->type != BL_PACKET_TYPE_COMMAND)
    {
        return false;
    }

    if (packet->length < BL_PROTOCOL_MIN_LENGTH)
    {
        return false;
    }

    return (packet->command == (uint8_t)command);
}


/* ============================================================================
 * Extract Command
 * ========================================================================== */

bool BL_Protocol_ExtractCommand(
    const BL_Protocol_Packet_t *packet,
    BL_Command_t               *command
)
{
    if ((packet == NULL) ||
        (command == NULL))
    {
        return false;
    }

    if (packet->type != BL_PACKET_TYPE_COMMAND)
    {
        return false;
    }

    if (packet->length < BL_PROTOCOL_MIN_LENGTH)
    {
        return false;
    }

    *command = (BL_Command_t)packet->command;

    return true;
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

    packet->sof     = BL_PROTOCOL_SOF;
    packet->type    = BL_PACKET_TYPE_COMMAND;
    packet->command = (uint8_t)command;

    /*
     * LENGTH:
     *
     * TYPE + COMMAND
     * = 1 + 1
     * = 2
     */

    packet->length = BL_PROTOCOL_FIXED_DATA_SIZE;

    packet->crc =
        BL_Protocol_ComputeCRC(packet);
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

    packet->sof     = BL_PROTOCOL_SOF;
    packet->type    = BL_PACKET_TYPE_DATA;
    packet->command = BL_CMD_NONE;

    /*
     * LENGTH:
     *
     * TYPE + COMMAND + DATA
     * = 2 + DATA length
     */

    packet->length =
        (uint8_t)(BL_PROTOCOL_FIXED_DATA_SIZE + length);

    for (uint8_t i = 0U; i < length; i++)
    {
        packet->data[i] = data[i];
    }

    packet->crc =
        BL_Protocol_ComputeCRC(packet);
}


/* ============================================================================
 * Extract Data
 * ========================================================================== */

bool BL_Protocol_ExtractData(
    const BL_Protocol_Packet_t *packet,
    uint8_t                    *data,
    uint8_t                    *length
)
{
    uint8_t data_length;

    if ((packet == NULL) ||
        (data == NULL) ||
        (length == NULL))
    {
        return false;
    }

    if (packet->type != BL_PACKET_TYPE_DATA)
    {
        return false;
    }

    if (packet->length < BL_PROTOCOL_MIN_LENGTH)
    {
        return false;
    }

    data_length =
        (uint8_t)(packet->length -
                  BL_PROTOCOL_FIXED_DATA_SIZE);

    for (uint8_t i = 0U; i < data_length; i++)
    {
        data[i] = packet->data[i];
    }

    *length = data_length;

    return true;
}