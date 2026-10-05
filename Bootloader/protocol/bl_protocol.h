#ifndef BL_PROTOCOL_H
#define BL_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

#include "cdefs/cdefs.h"


/* ============================================================================
 * Configuration
 * ========================================================================== */

#define BL_PROTOCOL_SOF            (0xA5U)

#define BL_PROTOCOL_MAX_DATA_SIZE  (16U)
#define BL_PROTOCOL_MAX_FRAME_SIZE (22U)


/* ============================================================================
 * Packet Size and Length
 *
 * LENGTH represents:
 *
 *     TYPE + COMMAND + DATA
 *
 * Wire Format:
 *
 * +------+--------+------+---------+------------+----------+
 * | SOF  | LENGTH | TYPE | COMMAND | DATA       | CRC-16   |
 * | 1 B  | 1 B    | 1 B  | 1 B     | 0..16 B    | 2 B      |
 * +------+--------+------+---------+------------+----------+
 *
 * LENGTH = TYPE + COMMAND + DATA
 *        = 2 + DATA_SIZE
 *
 * Total Frame Size = 1 + 1 + LENGTH + 2
 *                  = 4 + LENGTH
 *
 * Minimum Frame Size = 6 bytes
 * Maximum Frame Size = 22 bytes
 * ========================================================================== */

#define BL_PROTOCOL_HEADER_SIZE       (2U)

#define BL_PROTOCOL_TYPE_SIZE         (1U)

#define BL_PROTOCOL_COMMAND_SIZE      (1U)

#define BL_PROTOCOL_CRC_SIZE          (2U)

#define BL_PROTOCOL_FIXED_DATA_SIZE   \
    (BL_PROTOCOL_TYPE_SIZE + BL_PROTOCOL_COMMAND_SIZE)

#define BL_PROTOCOL_OVERHEAD_SIZE     \
    (BL_PROTOCOL_HEADER_SIZE + BL_PROTOCOL_CRC_SIZE)

#define BL_PROTOCOL_MIN_LENGTH        \
    (BL_PROTOCOL_FIXED_DATA_SIZE)

#define BL_PROTOCOL_MAX_LENGTH        \
    (BL_PROTOCOL_FIXED_DATA_SIZE + BL_PROTOCOL_MAX_DATA_SIZE)

#define BL_PROTOCOL_MIN_FRAME_SIZE    \
    (BL_PROTOCOL_OVERHEAD_SIZE + BL_PROTOCOL_MIN_LENGTH)

#define BL_PROTOCOL_MAX_FRAME_SIZE    \
    (BL_PROTOCOL_OVERHEAD_SIZE + BL_PROTOCOL_MAX_LENGTH)


/* ============================================================================
 * Packet Types
 * ========================================================================== */

typedef enum
{
    BL_PACKET_TYPE_COMMAND = 0x01U,
    BL_PACKET_TYPE_DATA    = 0x02U

} BL_PacketType_t;


/* ============================================================================
 * Protocol Commands
 * ========================================================================== */

typedef enum
{
    BL_CMD_NONE              = 0x00U,

    BL_CMD_SYNC_OBSERVED     = 0x20U,

    BL_CMD_FW_UPDATE_REQ     = 0x31U,
    BL_CMD_FW_UPDATE_RES     = 0x37U,

    BL_CMD_DEVICE_ID_REQ     = 0x3CU,
    BL_CMD_DEVICE_ID_RES     = 0x3FU,

    BL_CMD_FW_SIZE           = 0x42U,
    BL_CMD_FW_OVER_SIZE      = 0x45U,

    BL_CMD_READY_FOR_DATA    = 0x48U,

    BL_CMD_UPDATE_SUCCESSFUL = 0x54U,

    BL_CMD_ACK               = 0x15U,
    BL_CMD_NACK              = 0x59U,
    BL_CMD_RETX              = 0x19U

} BL_Command_t;


/* ============================================================================
 * Packet
 * ========================================================================== */

typedef struct
{
    uint8_t  sof;
    uint8_t  length;

    uint8_t  type;
    uint8_t  command;

    uint8_t  data[BL_PROTOCOL_MAX_DATA_SIZE];

    uint16_t crc;

} BL_Protocol_Packet_t;



typedef enum
{
    BL_PROTOCOL_PARSE_WAIT_SOF = 0U,
    BL_PROTOCOL_PARSE_LENGTH,
    BL_PROTOCOL_PARSE_TYPE,
    BL_PROTOCOL_PARSE_COMMAND,
    BL_PROTOCOL_PARSE_DATA,
    BL_PROTOCOL_PARSE_CRC_HIGH,
    BL_PROTOCOL_PARSE_CRC_LOW

} BL_Protocol_ParseState_t;


typedef struct
{
    BL_Protocol_Packet_t      packet;

    BL_Protocol_ParseState_t  state;

    uint8_t                   data_index;
    uint8_t                   data_length;

} BL_Protocol_Parser_t;


void BL_Protocol_Parser_Init(
    BL_Protocol_Parser_t *parser
);


bool BL_Protocol_Parser_PushByte(
    BL_Protocol_Parser_t *parser,
    uint8_t byte
);



/* ============================================================================
 * Protocol API
 * ========================================================================== */

/**
 * @brief Convert a logical packet into wire-format bytes.
 *
 * @param packet       Packet to serialize.
 * @param buffer       Destination buffer.
 * @param buffer_size  Size of destination buffer.
 * @param length       Generated frame length.
 *
 * @return true if conversion succeeds, otherwise false.
 */
bool BL_Protocol_PacketToBytes(
    const BL_Protocol_Packet_t *packet,
    uint8_t                    *buffer,
    uint16_t                    buffer_size,
    uint16_t                   *length
);


/**
 * @brief Parse and validate a wire-format packet.
 *
 * Performs:
 *     - SOF validation
 *     - LENGTH validation
 *     - Frame-size validation
 *     - CRC-16 validation
 *
 * @param buffer         Received frame bytes.
 * @param buffer_length  Number of received bytes.
 * @param packet         Output packet.
 *
 * @return true if the packet is valid, otherwise false.
 */
bool BL_Protocol_BytesToPacket(
    const uint8_t        *buffer,
    uint16_t              buffer_length,
    BL_Protocol_Packet_t *packet
);


/**
 * @brief Check whether a packet contains a specific command.
 *
 * @param packet   Packet to check.
 * @param command  Command to compare.
 *
 * @return true if the packet is a command packet containing
 *         the specified command.
 */
bool BL_Protocol_IsCommand(
    const BL_Protocol_Packet_t *packet,
    BL_Command_t                command
);


/**
 * @brief Extract the command from a command packet.
 *
 * @param packet   Command packet.
 * @param command  Output command.
 *
 * @return true if a valid command is extracted, otherwise false.
 */
bool BL_Protocol_ExtractCommand(
    const BL_Protocol_Packet_t *packet,
    BL_Command_t               *command
);


/**
 * @brief Create a command packet.
 *
 * Creates a packet with:
 *
 *     TYPE    = BL_PACKET_TYPE_COMMAND
 *     COMMAND = specified command
 *     DATA    = empty
 *
 * @param packet   Output packet.
 * @param command  Command to encode.
 */
void BL_Protocol_CreateCommandPacket(
    BL_Protocol_Packet_t *packet,
    BL_Command_t          command
);


/**
 * @brief Create a data packet.
 *
 * Creates a packet with:
 *
 *     TYPE    = BL_PACKET_TYPE_DATA
 *     COMMAND = BL_CMD_NONE
 *     DATA    = specified payload
 *
 * @param packet   Output packet.
 * @param data     Payload data.
 * @param length   Payload length.
 */
void BL_Protocol_CreateDataPacket(
    BL_Protocol_Packet_t *packet,
    const uint8_t        *data,
    uint8_t               length
);



/**
 * @brief Create a command packet with payload data.
 *
 * Creates a packet with:
 *
 *     SOF     = BL_PROTOCOL_SOF
 *     TYPE    = BL_PACKET_TYPE_COMMAND
 *     COMMAND = specified command
 *     DATA    = specified payload
 *
 * LENGTH = TYPE + COMMAND + DATA
 *
 * @param packet   Output packet.
 * @param command  Command to encode.
 * @param data     Payload data.
 * @param length   Payload data length.
 *
 * @return true if packet creation succeeds,
 *         otherwise false.
 */
bool BL_Protocol_CreateCommandDataPacket(
    BL_Protocol_Packet_t *packet,
    BL_Command_t          command,
    const uint8_t         *data,
    uint8_t               length
);


/**
 * @brief Extract data from a data packet.
 *
 * @param packet   Data packet.
 * @param data     Output data buffer.
 * @param length   Output data length.
 *
 * @return true if valid data is extracted, otherwise false.
 */

bool BL_Protocol_ExtractData(
    const BL_Protocol_Packet_t *packet,
    uint8_t                    *data,
    uint8_t                    *length
);


#endif /* BL_PROTOCOL_H */