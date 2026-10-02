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


/* ============================================================================
 * Packet Size and Length
 *
 * LENGTH represents DATA size only.
 *
 * Wire Format:
 *
 * +------+--------+------------------+----------+
 * | SOF  | LENGTH | DATA             | CRC-16   |
 * | 1 B  | 1 B    | 0..16 B          | 2 B      |
 * +------+--------+------------------+----------+
 *
 * Total Frame Size = 1 + 1 + LENGTH + 2
 *                  = 4 + LENGTH
 * ========================================================================== */

#define BL_PROTOCOL_HEADER_SIZE     (2U)
#define BL_PROTOCOL_CRC_SIZE        (2U)

#define BL_PROTOCOL_OVERHEAD_SIZE   \
    (BL_PROTOCOL_HEADER_SIZE + BL_PROTOCOL_CRC_SIZE)

#define BL_PROTOCOL_MIN_FRAME_SIZE  \
    (BL_PROTOCOL_OVERHEAD_SIZE)

#define BL_PROTOCOL_MAX_FRAME_SIZE  \
    (BL_PROTOCOL_OVERHEAD_SIZE + BL_PROTOCOL_MAX_DATA_SIZE)


/* ============================================================================
 * Protocol Commands
 * ========================================================================== */

typedef enum
{
    BL_CMD_SYNC_OBSERVED      = 0x20U,

    BL_CMD_FW_UPDATE_REQ      = 0x31U,
    BL_CMD_FW_UPDATE_RES      = 0x37U,

    BL_CMD_DEVICE_ID_REQ      = 0x3CU,
    BL_CMD_DEVICE_ID_RES      = 0x3FU,

    BL_CMD_FW_LENGTH_REQ      = 0x42U,
    BL_CMD_FW_LENGTH_RES      = 0x45U,

    BL_CMD_READY_FOR_DATA     = 0x48U,

    BL_CMD_UPDATE_SUCCESSFUL  = 0x54U,

    BL_CMD_ACK                = 0x15U,
    BL_CMD_NACK               = 0x59U,
    BL_CMD_RETX               = 0x19U

} BL_Command_t;


/* ============================================================================
 * Packet
 * ========================================================================== */

typedef struct
{
    uint8_t  sof;
    uint8_t  length;
    uint8_t  data[BL_PROTOCOL_MAX_DATA_SIZE];
    uint16_t crc;

} BL_Protocol_Packet_t;


/* ============================================================================
 * Protocol API
 * ========================================================================== */

bool BL_Protocol_PacketToBytes(
    const BL_Protocol_Packet_t *packet,
    uint8_t                    *buffer,
    uint16_t                    buffer_size,
    uint16_t                   *length
);

bool BL_Protocol_BytesToPacket(
    const uint8_t        *buffer,
    uint16_t              buffer_length,
    BL_Protocol_Packet_t *packet
);

bool BL_Protocol_IsCommand(
    const BL_Protocol_Packet_t *packet,
    BL_Command_t                command
);

void BL_Protocol_CreateCommandPacket(
    BL_Protocol_Packet_t *packet,
    BL_Command_t          command
);

void BL_Protocol_CreateDataPacket(
    BL_Protocol_Packet_t *packet,
    const uint8_t        *data,
    uint8_t               length
);


#endif /* BL_PROTOCOL_H */

