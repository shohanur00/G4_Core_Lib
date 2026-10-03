#include "../protocol/bl_protocol.h"
#include "bl_protocol_test.h"
#include "logger/frontend/logger.h"

#include <stdint.h>
#include <stdbool.h>


/* ============================================================================
 * Private Definitions
 * ========================================================================== */

#define TEST_LOG_MODULE     BL_PROTOCOL_TEST_LOG_MODULE


/* ============================================================================
 * Private Functions
 * ========================================================================== */

static bool BL_Protocol_Test_CommandPacket(void)
{
    BL_Protocol_Packet_t packet;
    BL_Protocol_Packet_t received_packet;

    BL_Command_t extracted_command;

    uint8_t  buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;


    /* ------------------------------------------------------------------------
     * Create Command Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_PROTOCOL_TEST_VALID_COMMAND
    );

    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Command created: CMD=0x%X LENGTH=%u CRC=0x%X",
        packet.command,
        packet.length,
        packet.crc
    );


    /* ------------------------------------------------------------------------
     * Validate Created Packet
     * ---------------------------------------------------------------------- */

    if (packet.sof != BL_PROTOCOL_SOF)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command SOF mismatch"
        );

        return false;
    }

    if (packet.type != BL_PACKET_TYPE_COMMAND)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command TYPE mismatch"
        );

        return false;
    }

    if (packet.length != BL_PROTOCOL_FIXED_DATA_SIZE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command LENGTH mismatch"
        );

        return false;
    }

    if (packet.command !=
        (uint8_t)BL_PROTOCOL_TEST_VALID_COMMAND)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command value mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Packet -> Bytes
     * ---------------------------------------------------------------------- */

    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command PacketToBytes FAILED"
        );

        return false;
    }

    if (buffer_length !=
        (BL_PROTOCOL_OVERHEAD_SIZE + packet.length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command frame length mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Serialized Frame
     * ---------------------------------------------------------------------- */

    if (buffer[0U] != BL_PROTOCOL_SOF)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Serialized SOF mismatch"
        );

        return false;
    }

    if (buffer[1U] != packet.length)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Serialized LENGTH mismatch"
        );

        return false;
    }

    if (buffer[2U] != packet.type)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Serialized TYPE mismatch"
        );

        return false;
    }

    if (buffer[3U] != packet.command)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Serialized command mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Bytes -> Packet
     * ---------------------------------------------------------------------- */

    if (!BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &received_packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command BytesToPacket FAILED"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Command
     * ---------------------------------------------------------------------- */

    if (!BL_Protocol_IsCommand(
            &received_packet,
            BL_PROTOCOL_TEST_VALID_COMMAND))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command validation FAILED"
        );

        return false;
    }

    if (!BL_Protocol_ExtractCommand(
            &received_packet,
            &extracted_command))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command extraction FAILED"
        );

        return false;
    }

    if (extracted_command != BL_PROTOCOL_TEST_VALID_COMMAND)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Extracted command mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Parsed Packet
     * ---------------------------------------------------------------------- */

    if (received_packet.sof != packet.sof)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command SOF mismatch after parse"
        );

        return false;
    }

    if (received_packet.length != packet.length)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command LENGTH mismatch after parse"
        );

        return false;
    }

    if (received_packet.type != packet.type)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command TYPE mismatch after parse"
        );

        return false;
    }

    if (received_packet.command != packet.command)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command value mismatch after parse"
        );

        return false;
    }

    if (received_packet.crc != packet.crc)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command CRC mismatch"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Command packet test PASSED"
    );

    return true;
}


/* ============================================================================
 * Data Packet Test
 * ========================================================================== */

static bool BL_Protocol_Test_DataPacket(void)
{
    BL_Protocol_Packet_t packet;
    BL_Protocol_Packet_t received_packet;

    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];

    const uint8_t test_data[BL_PROTOCOL_TEST_DATA_SIZE] =
        BL_PROTOCOL_TEST_DATA_INIT;

    uint16_t buffer_length;


    /* ------------------------------------------------------------------------
     * Create Data Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateDataPacket(
        &packet,
        test_data,
        sizeof(test_data)
    );

    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Data created: LENGTH=%u CRC=0x%X",
        packet.length,
        packet.crc
    );


    /* ------------------------------------------------------------------------
     * Validate Created Packet
     * ---------------------------------------------------------------------- */

    if (packet.sof != BL_PROTOCOL_SOF)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data SOF mismatch"
        );

        return false;
    }

    if (packet.type != BL_PACKET_TYPE_DATA)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data TYPE mismatch"
        );

        return false;
    }

    if (packet.command != BL_CMD_NONE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data COMMAND mismatch"
        );

        return false;
    }

    if (packet.length !=
        (BL_PROTOCOL_FIXED_DATA_SIZE + sizeof(test_data)))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data LENGTH mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Data
     * ---------------------------------------------------------------------- */

    for (uint16_t i = 0U; i < sizeof(test_data); i++)
    {
        if (packet.data[i] != test_data[i])
        {
            LOG_ERROR(
                TEST_LOG_MODULE,
                "Created data mismatch at index %u",
                i
            );

            return false;
        }
    }


    /* ------------------------------------------------------------------------
     * Packet -> Bytes
     * ---------------------------------------------------------------------- */

    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data PacketToBytes FAILED"
        );

        return false;
    }

    if (buffer_length !=
        (BL_PROTOCOL_OVERHEAD_SIZE + packet.length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data frame length mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Serialized Frame
     * ---------------------------------------------------------------------- */

    if (buffer[0U] != BL_PROTOCOL_SOF)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Serialized data SOF mismatch"
        );

        return false;
    }

    if (buffer[1U] != packet.length)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Serialized data LENGTH mismatch"
        );

        return false;
    }

    if (buffer[2U] != packet.type)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Serialized data TYPE mismatch"
        );

        return false;
    }

    if (buffer[3U] != packet.command)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Serialized data COMMAND mismatch"
        );

        return false;
    }

    for (uint16_t i = 0U; i < sizeof(test_data); i++)
    {
        if (buffer[4U + i] != test_data[i])
        {
            LOG_ERROR(
                TEST_LOG_MODULE,
                "Serialized data mismatch at index %u",
                i
            );

            return false;
        }
    }


    /* ------------------------------------------------------------------------
     * Bytes -> Packet
     * ---------------------------------------------------------------------- */

    if (!BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &received_packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data BytesToPacket FAILED"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Parsed Packet
     * ---------------------------------------------------------------------- */

    if (received_packet.sof != packet.sof)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data SOF mismatch after parse"
        );

        return false;
    }

    if (received_packet.length != packet.length)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data LENGTH mismatch after parse"
        );

        return false;
    }

    if (received_packet.type != packet.type)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data TYPE mismatch after parse"
        );

        return false;
    }

    if (received_packet.command != BL_CMD_NONE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data COMMAND mismatch after parse"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Data
     * ---------------------------------------------------------------------- */

    for (uint16_t i = 0U; i < sizeof(test_data); i++)
    {
        if (received_packet.data[i] != test_data[i])
        {
            LOG_ERROR(
                TEST_LOG_MODULE,
                "Data mismatch at index %u",
                i
            );

            return false;
        }
    }


    /* ------------------------------------------------------------------------
     * Validate CRC
     * ---------------------------------------------------------------------- */

    if (received_packet.crc != packet.crc)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data CRC mismatch"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Data packet test PASSED"
    );

    return true;
}


/* ============================================================================
 * Zero-Length Packet Test
 * ========================================================================== */

static bool BL_Protocol_Test_ZeroLengthPacket(void)
{
    BL_Protocol_Packet_t packet;
    BL_Protocol_Packet_t received_packet;

    uint8_t  buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;


    BL_Protocol_CreateDataPacket(
        &packet,
        NULL,
        0U
    );


    if (packet.sof != BL_PROTOCOL_SOF)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length SOF mismatch"
        );

        return false;
    }

    if (packet.type != BL_PACKET_TYPE_DATA)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length TYPE mismatch"
        );

        return false;
    }

    if (packet.command != BL_CMD_NONE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length COMMAND mismatch"
        );

        return false;
    }

    if (packet.length != BL_PROTOCOL_FIXED_DATA_SIZE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length LENGTH mismatch"
        );

        return false;
    }


    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length PacketToBytes FAILED"
        );

        return false;
    }

    if (buffer_length != BL_PROTOCOL_MIN_FRAME_SIZE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length frame size mismatch"
        );

        return false;
    }


    if (!BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &received_packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length BytesToPacket FAILED"
        );

        return false;
    }


    if (received_packet.length !=
        BL_PROTOCOL_FIXED_DATA_SIZE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length parsed LENGTH mismatch"
        );

        return false;
    }

    if (received_packet.type != BL_PACKET_TYPE_DATA)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length parsed TYPE mismatch"
        );

        return false;
    }

    if (received_packet.command != BL_CMD_NONE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length parsed COMMAND mismatch"
        );

        return false;
    }

    if (received_packet.crc != packet.crc)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Zero-length CRC mismatch"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Zero-length packet test PASSED"
    );

    return true;
}


/* ============================================================================
 * Maximum-Length Packet Test
 * ========================================================================== */

static bool BL_Protocol_Test_MaxLengthPacket(void)
{
    BL_Protocol_Packet_t packet;
    BL_Protocol_Packet_t received_packet;

    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint8_t test_data[BL_PROTOCOL_MAX_DATA_SIZE];

    uint16_t buffer_length;


    /* ------------------------------------------------------------------------
     * Prepare Maximum-Length Data
     * ---------------------------------------------------------------------- */

    for (uint16_t i = 0U;
         i < BL_PROTOCOL_MAX_DATA_SIZE;
         i++)
    {
        test_data[i] = (uint8_t)i;
    }


    /* ------------------------------------------------------------------------
     * Create Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateDataPacket(
        &packet,
        test_data,
        BL_PROTOCOL_MAX_DATA_SIZE
    );


    /* ------------------------------------------------------------------------
     * Validate Packet
     * ---------------------------------------------------------------------- */

    if (packet.length != BL_PROTOCOL_MAX_LENGTH)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum LENGTH mismatch"
        );

        return false;
    }

    if (packet.type != BL_PACKET_TYPE_DATA)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum TYPE mismatch"
        );

        return false;
    }

    if (packet.command != BL_CMD_NONE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum COMMAND mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Data
     * ---------------------------------------------------------------------- */

    for (uint16_t i = 0U;
         i < BL_PROTOCOL_MAX_DATA_SIZE;
         i++)
    {
        if (packet.data[i] != test_data[i])
        {
            LOG_ERROR(
                TEST_LOG_MODULE,
                "Maximum data mismatch at index %u",
                i
            );

            return false;
        }
    }


    /* ------------------------------------------------------------------------
     * Serialize
     * ---------------------------------------------------------------------- */

    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum PacketToBytes FAILED"
        );

        return false;
    }

    if (buffer_length != BL_PROTOCOL_MAX_FRAME_SIZE)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum frame size mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Parse
     * ---------------------------------------------------------------------- */

    if (!BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &received_packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum BytesToPacket FAILED"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Parsed Packet
     * ---------------------------------------------------------------------- */

    if (received_packet.length != packet.length)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum parsed LENGTH mismatch"
        );

        return false;
    }

    if (received_packet.type != packet.type)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum parsed TYPE mismatch"
        );

        return false;
    }

    if (received_packet.command != packet.command)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum parsed COMMAND mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Validate Data
     * ---------------------------------------------------------------------- */

    for (uint16_t i = 0U;
         i < BL_PROTOCOL_MAX_DATA_SIZE;
         i++)
    {
        if (received_packet.data[i] != test_data[i])
        {
            LOG_ERROR(
                TEST_LOG_MODULE,
                "Maximum parsed data mismatch at index %u",
                i
            );

            return false;
        }
    }


    /* ------------------------------------------------------------------------
     * Validate CRC
     * ---------------------------------------------------------------------- */

    if (received_packet.crc != packet.crc)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Maximum CRC mismatch"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Maximum-length packet test PASSED"
    );

    return true;
}


/* ============================================================================
 * CRC Corruption Test
 * ========================================================================== */

static bool BL_Protocol_Test_CRCCorruption(void)
{
    BL_Protocol_Packet_t packet;

    uint8_t  buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;


    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_PROTOCOL_TEST_VALID_COMMAND
    );


    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "CRC corruption PacketToBytes FAILED"
        );

        return false;
    }


    /* Corrupt command byte */
    buffer[3U] ^= 0x01U;


    if (BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "CRC corruption was NOT detected"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "CRC corruption test PASSED"
    );

    return true;
}


/* ============================================================================
 * Invalid SOF Test
 * ========================================================================== */

static bool BL_Protocol_Test_InvalidSOF(void)
{
    BL_Protocol_Packet_t packet;

    uint8_t  buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;


    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_PROTOCOL_TEST_VALID_COMMAND
    );


    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Invalid SOF PacketToBytes FAILED"
        );

        return false;
    }


    buffer[0U] ^= 0xFFU;


    if (BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Invalid SOF was NOT detected"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Invalid SOF test PASSED"
    );

    return true;
}


/* ============================================================================
 * Invalid Length Test
 * ========================================================================== */

static bool BL_Protocol_Test_InvalidLength(void)
{
    BL_Protocol_Packet_t packet;

    uint8_t  buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;


    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_PROTOCOL_TEST_VALID_COMMAND
    );


    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Invalid LENGTH PacketToBytes FAILED"
        );

        return false;
    }


    buffer[1U] = BL_PROTOCOL_MAX_LENGTH + 1U;


    if (BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Invalid LENGTH was NOT detected"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Invalid LENGTH test PASSED"
    );

    return true;
}


/* ============================================================================
 * Short Frame Test
 * ========================================================================== */

static bool BL_Protocol_Test_ShortFrame(void)
{
    BL_Protocol_Packet_t packet;

    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];


    for (uint16_t i = 0U;
         i < BL_PROTOCOL_MIN_FRAME_SIZE - 1U;
         i++)
    {
        buffer[i] = 0U;
    }


    if (BL_Protocol_BytesToPacket(
            buffer,
            BL_PROTOCOL_MIN_FRAME_SIZE - 1U,
            &packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Short frame was NOT rejected"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Short frame test PASSED"
    );

    return true;
}


/* ============================================================================
 * Truncated Frame Test
 * ========================================================================== */

static bool BL_Protocol_Test_TruncatedFrame(void)
{
    BL_Protocol_Packet_t packet;

    uint8_t  buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;

    static const uint8_t test_data[] =
    {
        0x11U,
        0x22U,
        0x33U,
        0x44U
    };


    BL_Protocol_CreateDataPacket(
        &packet,
        test_data,
        sizeof(test_data)
    );


    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Truncated frame PacketToBytes FAILED"
        );

        return false;
    }


    if (BL_Protocol_BytesToPacket(
            buffer,
            buffer_length - 1U,
            &packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Truncated frame was NOT rejected"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Truncated frame test PASSED"
    );

    return true;
}


/* ============================================================================
 * Small Buffer Test
 * ========================================================================== */

static bool BL_Protocol_Test_SmallBuffer(void)
{
    BL_Protocol_Packet_t packet;

    uint8_t  buffer[BL_PROTOCOL_MIN_FRAME_SIZE - 1U];
    uint16_t buffer_length;


    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_PROTOCOL_TEST_VALID_COMMAND
    );


    if (BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Small buffer was NOT rejected"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Small buffer test PASSED"
    );

    return true;
}


/* ============================================================================
 * Invalid Command Test
 * ========================================================================== */

static bool BL_Protocol_Test_InvalidCommand(void)
{
    BL_Protocol_Packet_t packet;


    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_PROTOCOL_TEST_VALID_COMMAND
    );


    if (BL_Protocol_IsCommand(
            &packet,
            BL_PROTOCOL_TEST_INVALID_COMMAND))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Invalid command validation FAILED"
        );

        return false;
    }


    if (!BL_Protocol_IsCommand(
            &packet,
            BL_PROTOCOL_TEST_VALID_COMMAND))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Valid command validation FAILED"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Command validation test PASSED"
    );

    return true;
}


/* ============================================================================
 * Extract Command Test
 * ========================================================================== */

static bool BL_Protocol_Test_ExtractCommand(void)
{
    BL_Protocol_Packet_t command_packet;
    BL_Protocol_Packet_t data_packet;

    BL_Command_t command;


    /* ------------------------------------------------------------------------
     * Valid Command Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateCommandPacket(
        &command_packet,
        BL_PROTOCOL_TEST_VALID_COMMAND
    );


    if (!BL_Protocol_ExtractCommand(
            &command_packet,
            &command))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Command extraction FAILED"
        );

        return false;
    }


    if (command != BL_PROTOCOL_TEST_VALID_COMMAND)
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Extracted command mismatch"
        );

        return false;
    }


    /* ------------------------------------------------------------------------
     * Data Packet Must Not Be Treated As Command
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateDataPacket(
        &data_packet,
        NULL,
        0U
    );


    if (BL_Protocol_ExtractCommand(
            &data_packet,
            &command))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "Data packet accepted as command"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Extract command test PASSED"
    );

    return true;
}


/* ============================================================================
 * All Command Test
 * ========================================================================== */

static bool BL_Protocol_Test_AllCommands(void)
{
    static const BL_Command_t commands[] =
        BL_PROTOCOL_TEST_COMMAND_LIST;

    BL_Protocol_Packet_t packet;

    const uint32_t command_count =
        sizeof(commands) / sizeof(commands[0]);


    for (uint32_t i = 0U; i < command_count; i++)
    {
        BL_Protocol_CreateCommandPacket(
            &packet,
            commands[i]
        );


        if (packet.length != BL_PROTOCOL_FIXED_DATA_SIZE)
        {
            LOG_ERROR(
                TEST_LOG_MODULE,
                "Command LENGTH FAILED: index=%u",
                i
            );

            return false;
        }


        if (packet.type != BL_PACKET_TYPE_COMMAND)
        {
            LOG_ERROR(
                TEST_LOG_MODULE,
                "Command TYPE FAILED: index=%u",
                i
            );

            return false;
        }


        if (packet.command != (uint8_t)commands[i])
        {
            LOG_ERROR(
                TEST_LOG_MODULE,
                "Command value FAILED: index=%u",
                i
            );

            return false;
        }


        if (!BL_Protocol_IsCommand(
                &packet,
                commands[i]))
        {
            LOG_ERROR(
                TEST_LOG_MODULE,
                "Command validation FAILED: index=%u",
                i
            );

            return false;
        }
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "All command test PASSED"
    );

    return true;
}


/* ============================================================================
 * NULL Parameter Test
 * ========================================================================== */

static bool BL_Protocol_Test_NullParameters(void)
{
    BL_Protocol_Packet_t packet;

    uint8_t  buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;

    BL_Command_t command;


    /* PacketToBytes: NULL packet */

    if (BL_Protocol_PacketToBytes(
            NULL,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "NULL packet was NOT rejected"
        );

        return false;
    }


    /* PacketToBytes: NULL buffer */

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_PROTOCOL_TEST_VALID_COMMAND
    );


    if (BL_Protocol_PacketToBytes(
            &packet,
            NULL,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "NULL buffer was NOT rejected"
        );

        return false;
    }


    /* PacketToBytes: NULL length */

    if (BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            NULL))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "NULL length was NOT rejected"
        );

        return false;
    }


    /* BytesToPacket: NULL input buffer */

    if (BL_Protocol_BytesToPacket(
            NULL,
            BL_PROTOCOL_MIN_FRAME_SIZE,
            &packet))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "NULL input buffer was NOT rejected"
        );

        return false;
    }


    /* BytesToPacket: NULL output packet */

    if (BL_Protocol_BytesToPacket(
            buffer,
            BL_PROTOCOL_MIN_FRAME_SIZE,
            NULL))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "NULL output packet was NOT rejected"
        );

        return false;
    }


    /* IsCommand: NULL packet */

    if (BL_Protocol_IsCommand(
            NULL,
            BL_CMD_ACK))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "NULL command packet was NOT rejected"
        );

        return false;
    }


    /* ExtractCommand: NULL packet */

    if (BL_Protocol_ExtractCommand(
            NULL,
            &command))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "NULL extract packet was NOT rejected"
        );

        return false;
    }


    /* ExtractCommand: NULL command */

    if (BL_Protocol_ExtractCommand(
            &packet,
            NULL))
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "NULL extract command was NOT rejected"
        );

        return false;
    }


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "NULL parameter test PASSED"
    );

    return true;
}


/* ============================================================================
 * Public Function
 * ========================================================================== */

void BL_Protocol_Test(void)
{
    bool test_result = true;


    LOG_DEBUG(
        TEST_LOG_MODULE,
        "========================================"
    );

    LOG_DEBUG(
        TEST_LOG_MODULE,
        "Protocol test started"
    );

    LOG_DEBUG(
        TEST_LOG_MODULE,
        "========================================"
    );


    /* ------------------------------------------------------------------------
     * Functional Tests
     * ---------------------------------------------------------------------- */

#if BL_PROTOCOL_TEST_ENABLE_COMMAND

    if (!BL_Protocol_Test_CommandPacket())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_DATA

    if (!BL_Protocol_Test_DataPacket())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_ZERO_LENGTH

    if (!BL_Protocol_Test_ZeroLengthPacket())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_MAX_LENGTH

    if (!BL_Protocol_Test_MaxLengthPacket())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_ALL_COMMANDS

    if (!BL_Protocol_Test_AllCommands())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_INVALID_COMMAND

    if (!BL_Protocol_Test_InvalidCommand())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_EXTRACT_COMMAND

    if (!BL_Protocol_Test_ExtractCommand())
    {
        test_result = false;
    }

#endif


    /* ------------------------------------------------------------------------
     * Error Detection Tests
     * ---------------------------------------------------------------------- */

#if BL_PROTOCOL_TEST_ENABLE_CRC

    if (!BL_Protocol_Test_CRCCorruption())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_INVALID_SOF

    if (!BL_Protocol_Test_InvalidSOF())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_INVALID_LENGTH

    if (!BL_Protocol_Test_InvalidLength())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_SHORT_FRAME

    if (!BL_Protocol_Test_ShortFrame())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_TRUNCATED

    if (!BL_Protocol_Test_TruncatedFrame())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_SMALL_BUFFER

    if (!BL_Protocol_Test_SmallBuffer())
    {
        test_result = false;
    }

#endif


#if BL_PROTOCOL_TEST_ENABLE_NULL_PARAMETER

    if (!BL_Protocol_Test_NullParameters())
    {
        test_result = false;
    }

#endif


    /* ------------------------------------------------------------------------
     * Final Result
     * ---------------------------------------------------------------------- */

    if (test_result)
    {
        LOG_DEBUG(
            TEST_LOG_MODULE,
            "========================================"
        );

        LOG_DEBUG(
            TEST_LOG_MODULE,
            "Protocol test PASSED"
        );

        LOG_DEBUG(
            TEST_LOG_MODULE,
            "========================================"
        );
    }
    else
    {
        LOG_ERROR(
            TEST_LOG_MODULE,
            "========================================"
        );

        LOG_ERROR(
            TEST_LOG_MODULE,
            "Protocol test FAILED"
        );

        LOG_ERROR(
            TEST_LOG_MODULE,
            "========================================"
        );
    }
}