#include "../protocol/bl_protocol.h"
#include "bl_protocol_test.h"
#include "logger/frontend/logger.h"

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Private Definitions
 * ========================================================================== */

#define BL_PROTOCOL_TEST_DATA_SIZE     (5U)

#define BL_PROTOCOL_TEST_DATA_0        (0x31U)
#define BL_PROTOCOL_TEST_DATA_1        (0x12U)
#define BL_PROTOCOL_TEST_DATA_2        (0x34U)
#define BL_PROTOCOL_TEST_DATA_3        (0x56U)
#define BL_PROTOCOL_TEST_DATA_4        (0x78U)


/* ============================================================================
 * Private Functions
 * ========================================================================== */

static bool BL_Protocol_Test_CommandPacket(void)
{
    BL_Protocol_Packet_t packet;
    BL_Protocol_Packet_t received_packet;

    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;

    /* ------------------------------------------------------------------------
     * Create Command Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_CMD_FW_UPDATE_REQ
    );

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
        "Command created: CMD=0x%X LENGTH=%u CRC=0x%X",
        packet.data[0],
        packet.length,
        packet.crc
    );

    /* ------------------------------------------------------------------------
     * Validate Created Packet
     * ---------------------------------------------------------------------- */

    if (packet.sof != BL_PROTOCOL_SOF)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Command SOF mismatch"
        );

        return false;
    }

    if (packet.length != 1U)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Command length mismatch"
        );

        return false;
    }

    if (packet.data[0] != (uint8_t)BL_CMD_FW_UPDATE_REQ)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
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
            LOG_MODULE_BOOTLOADER,
            "Command PacketToBytes FAILED"
        );

        return false;
    }

    if (buffer_length !=
        (BL_PROTOCOL_OVERHEAD_SIZE + packet.length))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Command frame length mismatch"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Validate Serialized Frame
     * ---------------------------------------------------------------------- */

    if (buffer[0] != BL_PROTOCOL_SOF)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Serialized SOF mismatch"
        );

        return false;
    }

    if (buffer[1] != packet.length)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Serialized LENGTH mismatch"
        );

        return false;
    }

    if (buffer[2] != packet.data[0])
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
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
            LOG_MODULE_BOOTLOADER,
            "Command BytesToPacket FAILED"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Validate Command
     * ---------------------------------------------------------------------- */

    if (!BL_Protocol_IsCommand(
            &received_packet,
            BL_CMD_FW_UPDATE_REQ))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Command validation FAILED"
        );

        return false;
    }

    if (received_packet.sof != packet.sof)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Command SOF mismatch after parse"
        );

        return false;
    }

    if (received_packet.length != packet.length)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Command length mismatch after parse"
        );

        return false;
    }

    if (received_packet.data[0] != packet.data[0])
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Command data mismatch after parse"
        );

        return false;
    }

    if (received_packet.crc != packet.crc)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Command CRC mismatch"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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
    {
        BL_PROTOCOL_TEST_DATA_0,
        BL_PROTOCOL_TEST_DATA_1,
        BL_PROTOCOL_TEST_DATA_2,
        BL_PROTOCOL_TEST_DATA_3,
        BL_PROTOCOL_TEST_DATA_4
    };

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
        LOG_MODULE_BOOTLOADER,
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
            LOG_MODULE_BOOTLOADER,
            "Data SOF mismatch"
        );

        return false;
    }

    if (packet.length != sizeof(test_data))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Data length mismatch"
        );

        return false;
    }

    for (uint8_t i = 0U; i < packet.length; i++)
    {
        if (packet.data[i] != test_data[i])
        {
            LOG_ERROR(
                LOG_MODULE_BOOTLOADER,
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
            LOG_MODULE_BOOTLOADER,
            "Data PacketToBytes FAILED"
        );

        return false;
    }

    if (buffer_length !=
        (BL_PROTOCOL_OVERHEAD_SIZE + packet.length))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Data frame length mismatch"
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
            LOG_MODULE_BOOTLOADER,
            "Data BytesToPacket FAILED"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Validate SOF
     * ---------------------------------------------------------------------- */

    if (received_packet.sof != packet.sof)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Data SOF mismatch after parse"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Validate Length
     * ---------------------------------------------------------------------- */

    if (received_packet.length != packet.length)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Data length mismatch after parse"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Validate Data
     * ---------------------------------------------------------------------- */

    for (uint8_t i = 0U; i < packet.length; i++)
    {
        if (received_packet.data[i] != packet.data[i])
        {
            LOG_ERROR(
                LOG_MODULE_BOOTLOADER,
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
            LOG_MODULE_BOOTLOADER,
            "Data CRC mismatch"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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

    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;

    /* ------------------------------------------------------------------------
     * Create Zero-Length Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateDataPacket(
        &packet,
        NULL,
        0U
    );

    if (packet.sof != BL_PROTOCOL_SOF)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Zero-length SOF mismatch"
        );

        return false;
    }

    if (packet.length != 0U)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Zero-length LENGTH mismatch"
        );

        return false;
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
            LOG_MODULE_BOOTLOADER,
            "Zero-length PacketToBytes FAILED"
        );

        return false;
    }

    if (buffer_length != BL_PROTOCOL_MIN_FRAME_SIZE)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Zero-length frame size mismatch"
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
            LOG_MODULE_BOOTLOADER,
            "Zero-length BytesToPacket FAILED"
        );

        return false;
    }

    if (received_packet.length != 0U)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Zero-length parsed LENGTH mismatch"
        );

        return false;
    }

    if (received_packet.crc != packet.crc)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Zero-length CRC mismatch"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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

    for (uint8_t i = 0U; i < BL_PROTOCOL_MAX_DATA_SIZE; i++)
    {
        test_data[i] = i;
    }

    /* ------------------------------------------------------------------------
     * Create Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateDataPacket(
        &packet,
        test_data,
        BL_PROTOCOL_MAX_DATA_SIZE
    );

    if (packet.length != BL_PROTOCOL_MAX_DATA_SIZE)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Maximum LENGTH mismatch"
        );

        return false;
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
            LOG_MODULE_BOOTLOADER,
            "Maximum PacketToBytes FAILED"
        );

        return false;
    }

    if (buffer_length != BL_PROTOCOL_MAX_FRAME_SIZE)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
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
            LOG_MODULE_BOOTLOADER,
            "Maximum BytesToPacket FAILED"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Validate Data
     * ---------------------------------------------------------------------- */

    for (uint8_t i = 0U; i < BL_PROTOCOL_MAX_DATA_SIZE; i++)
    {
        if (received_packet.data[i] != test_data[i])
        {
            LOG_ERROR(
                LOG_MODULE_BOOTLOADER,
                "Maximum data mismatch at index %u",
                i
            );

            return false;
        }
    }

    if (received_packet.crc != packet.crc)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Maximum CRC mismatch"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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

    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;

    /* ------------------------------------------------------------------------
     * Create Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_CMD_FW_UPDATE_REQ
    );

    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "CRC corruption PacketToBytes FAILED"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Corrupt Data
     * ---------------------------------------------------------------------- */

    buffer[2U] ^= 0x01U;

    /* ------------------------------------------------------------------------
     * Packet Must Be Rejected
     * ---------------------------------------------------------------------- */

    if (BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &packet))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "CRC corruption was NOT detected"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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

    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;

    /* ------------------------------------------------------------------------
     * Create Valid Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_CMD_FW_UPDATE_REQ
    );

    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Invalid SOF PacketToBytes FAILED"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Corrupt SOF
     * ---------------------------------------------------------------------- */

    buffer[0] ^= 0xFFU;

    /* ------------------------------------------------------------------------
     * Packet Must Be Rejected
     * ---------------------------------------------------------------------- */

    if (BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &packet))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Invalid SOF was NOT detected"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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

    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;

    /* ------------------------------------------------------------------------
     * Create Valid Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_CMD_FW_UPDATE_REQ
    );

    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Invalid LENGTH PacketToBytes FAILED"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Set LENGTH Beyond Maximum
     * ---------------------------------------------------------------------- */

    buffer[1] = BL_PROTOCOL_MAX_DATA_SIZE + 1U;

    if (BL_Protocol_BytesToPacket(
            buffer,
            buffer_length,
            &packet))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Invalid LENGTH was NOT detected"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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

    /* ------------------------------------------------------------------------
     * Frame Smaller Than Minimum
     * ---------------------------------------------------------------------- */

    for (uint8_t i = 0U; i < BL_PROTOCOL_MIN_FRAME_SIZE - 1U; i++)
    {
        buffer[i] = 0U;
    }

    if (BL_Protocol_BytesToPacket(
            buffer,
            BL_PROTOCOL_MIN_FRAME_SIZE - 1U,
            &packet))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Short frame was NOT rejected"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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

    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;

    /* ------------------------------------------------------------------------
     * Create Valid Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateDataPacket(
        &packet,
        (const uint8_t[]){0x11U, 0x22U, 0x33U, 0x44U},
        4U
    );

    if (!BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Truncated frame PacketToBytes FAILED"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * Remove Last Byte
     * ---------------------------------------------------------------------- */

    if (BL_Protocol_BytesToPacket(
            buffer,
            buffer_length - 1U,
            &packet))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Truncated frame was NOT rejected"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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

    uint8_t buffer[BL_PROTOCOL_MIN_FRAME_SIZE - 1U];
    uint16_t buffer_length;

    /* ------------------------------------------------------------------------
     * Create Packet
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_CMD_FW_UPDATE_REQ
    );

    /* ------------------------------------------------------------------------
     * Buffer Too Small
     * ---------------------------------------------------------------------- */

    if (BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Small buffer was NOT rejected"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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
        BL_CMD_FW_UPDATE_REQ
    );

    if (BL_Protocol_IsCommand(
            &packet,
            BL_CMD_DEVICE_ID_REQ))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Invalid command validation FAILED"
        );

        return false;
    }

    if (!BL_Protocol_IsCommand(
            &packet,
            BL_CMD_FW_UPDATE_REQ))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Valid command validation FAILED"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
        "Command validation test PASSED"
    );

    return true;
}


/* ============================================================================
 * All Command Test
 * ========================================================================== */

static bool BL_Protocol_Test_AllCommands(void)
{
    static const BL_Command_t commands[] =
    {
        BL_CMD_SYNC_OBSERVED,
        BL_CMD_FW_UPDATE_REQ,
        BL_CMD_FW_UPDATE_RES,
        BL_CMD_DEVICE_ID_REQ,
        BL_CMD_DEVICE_ID_RES,
        BL_CMD_FW_LENGTH_REQ,
        BL_CMD_FW_LENGTH_RES,
        BL_CMD_READY_FOR_DATA,
        BL_CMD_UPDATE_SUCCESSFUL,
        BL_CMD_ACK,
        BL_CMD_NACK,
        BL_CMD_RETX
    };

    BL_Protocol_Packet_t packet;

    uint32_t command_count =
        sizeof(commands) / sizeof(commands[0]);

    for (uint32_t i = 0U; i < command_count; i++)
    {
        BL_Protocol_CreateCommandPacket(
            &packet,
            commands[i]
        );

        if (packet.length != 1U)
        {
            LOG_ERROR(
                LOG_MODULE_BOOTLOADER,
                "Command length FAILED: index=%u",
                i
            );

            return false;
        }

        if (packet.data[0] != (uint8_t)commands[i])
        {
            LOG_ERROR(
                LOG_MODULE_BOOTLOADER,
                "Command data FAILED: index=%u",
                i
            );

            return false;
        }

        if (!BL_Protocol_IsCommand(
                &packet,
                commands[i]))
        {
            LOG_ERROR(
                LOG_MODULE_BOOTLOADER,
                "Command validation FAILED: index=%u",
                i
            );

            return false;
        }
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
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
    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t buffer_length;

    /* ------------------------------------------------------------------------
     * PacketToBytes NULL Packet
     * ---------------------------------------------------------------------- */

    if (BL_Protocol_PacketToBytes(
            NULL,
            buffer,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "NULL packet was NOT rejected"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * PacketToBytes NULL Buffer
     * ---------------------------------------------------------------------- */

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_CMD_FW_UPDATE_REQ
    );

    if (BL_Protocol_PacketToBytes(
            &packet,
            NULL,
            sizeof(buffer),
            &buffer_length))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "NULL buffer was NOT rejected"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * PacketToBytes NULL Length
     * ---------------------------------------------------------------------- */

    if (BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            NULL))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "NULL length was NOT rejected"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * BytesToPacket NULL Buffer
     * ---------------------------------------------------------------------- */

    if (BL_Protocol_BytesToPacket(
            NULL,
            BL_PROTOCOL_MIN_FRAME_SIZE,
            &packet))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "NULL input buffer was NOT rejected"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * BytesToPacket NULL Packet
     * ---------------------------------------------------------------------- */

    if (BL_Protocol_BytesToPacket(
            buffer,
            BL_PROTOCOL_MIN_FRAME_SIZE,
            NULL))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "NULL output packet was NOT rejected"
        );

        return false;
    }

    /* ------------------------------------------------------------------------
     * IsCommand NULL Packet
     * ---------------------------------------------------------------------- */

    if (BL_Protocol_IsCommand(
            NULL,
            BL_CMD_ACK))
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "NULL command packet was NOT rejected"
        );

        return false;
    }

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
        "NULL parameter test PASSED"
    );

    return true;
}


/* ============================================================================
 * Public Functions
 * ========================================================================== */

void BL_Protocol_Test(void)
{
    bool command_test;
    bool data_test;
    bool zero_length_test;
    bool maximum_length_test;
    bool crc_corruption_test;
    bool invalid_sof_test;
    bool invalid_length_test;
    bool short_frame_test;
    bool truncated_frame_test;
    bool small_buffer_test;
    bool invalid_command_test;
    bool all_commands_test;
    bool null_parameter_test;

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
        "========================================"
    );

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
        "Protocol test started"
    );

    LOG_DEBUG(
        LOG_MODULE_BOOTLOADER,
        "========================================"
    );

    /* ------------------------------------------------------------------------
     * Functional Tests
     * ---------------------------------------------------------------------- */

    command_test =
        BL_Protocol_Test_CommandPacket();

    data_test =
        BL_Protocol_Test_DataPacket();

    zero_length_test =
        BL_Protocol_Test_ZeroLengthPacket();

    maximum_length_test =
        BL_Protocol_Test_MaxLengthPacket();

    all_commands_test =
        BL_Protocol_Test_AllCommands();

    invalid_command_test =
        BL_Protocol_Test_InvalidCommand();

    /* ------------------------------------------------------------------------
     * Error Detection Tests
     * ---------------------------------------------------------------------- */

    crc_corruption_test =
        BL_Protocol_Test_CRCCorruption();

    invalid_sof_test =
        BL_Protocol_Test_InvalidSOF();

    invalid_length_test =
        BL_Protocol_Test_InvalidLength();

    short_frame_test =
        BL_Protocol_Test_ShortFrame();

    truncated_frame_test =
        BL_Protocol_Test_TruncatedFrame();

    small_buffer_test =
        BL_Protocol_Test_SmallBuffer();

    null_parameter_test =
        BL_Protocol_Test_NullParameters();

    /* ------------------------------------------------------------------------
     * Final Result
     * ---------------------------------------------------------------------- */

    if (command_test &&
        data_test &&
        zero_length_test &&
        maximum_length_test &&
        all_commands_test &&
        invalid_command_test &&
        crc_corruption_test &&
        invalid_sof_test &&
        invalid_length_test &&
        short_frame_test &&
        truncated_frame_test &&
        small_buffer_test &&
        null_parameter_test)
    {
        LOG_DEBUG(
            LOG_MODULE_BOOTLOADER,
            "========================================"
        );

        LOG_DEBUG(
            LOG_MODULE_BOOTLOADER,
            "Protocol test PASSED"
        );

        LOG_DEBUG(
            LOG_MODULE_BOOTLOADER,
            "========================================"
        );
    }
    else
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "========================================"
        );

        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "Protocol test FAILED"
        );

        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "========================================"
        );
    }
}
