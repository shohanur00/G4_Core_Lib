#include "bootloader.h"

#include "../transport/bl_transport.h"
#include "../protocol/bl_protocol.h"
#include "cdefs/cdefs.h"


static BL_Protocol_Parser_t protocol_parser;
static BL_Protocol_Packet_t packet;


void Bootloader_Init(void)
{
    BL_Protocol_Parser_Init(&protocol_parser);
    BL_Transport_Init();
}


static void Bootloader_SendACK(void)
{
    BL_Protocol_Packet_t packet;
    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t length;

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_CMD_ACK
    );

    if (BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &length))
    {
        BL_Transport_Write(
            buffer,
            length
        );
    }
}


static void Bootloader_SendNACK(void)
{
    BL_Protocol_Packet_t packet;
    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t length;

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_CMD_NACK
    );

    if (BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &length))
    {
        BL_Transport_Write(
            buffer,
            length
        );
    }
}


static void Bootloader_SendRETX(void)
{
    BL_Protocol_Packet_t packet;
    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t length;

    BL_Protocol_CreateCommandPacket(
        &packet,
        BL_CMD_RETX
    );

    if (BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &length))
    {
        BL_Transport_Write(
            buffer,
            length
        );
    }
}


static void Bootloader_ProcessPacket(
    const BL_Protocol_Packet_t *packet
)
{
    BL_Command_t command;

    if (!BL_Protocol_ExtractCommand(
            packet,
            &command))
    {
        Bootloader_SendNACK();
        return;
    }

    switch (command)
    {
        case BL_CMD_SYNC_OBSERVED:
            /* Handle SYNC */
            Bootloader_SendACK();
            break;


        case BL_CMD_FW_UPDATE_REQ:
            /* Handle firmware update request */
            break;


        case BL_CMD_DEVICE_ID_REQ:
            /* Handle device ID request */
            break;


        case BL_CMD_FW_LENGTH_REQ:
            /* Handle firmware length request */
            break;


        case BL_CMD_RETX:
            /* Handle retransmission request */
            break;


        default:
            /* Unknown command */
            Bootloader_SendNACK();
            break;
    }
}


void Bootloader_Process(void)
{
    uint8_t byte;

    while (BL_Transport_DataAvailable() > 0U)
    {
        if (BL_Transport_ReadByte(&byte))
        {
            if (BL_Protocol_Parser_PushByte(
                    &protocol_parser,
                    byte))
            {
                Bootloader_ProcessPacket(
                    &protocol_parser.packet
                );
            }
        }
    }
}