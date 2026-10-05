#include "bootloader.h"

#include "../transport/bl_transport.h"
#include "../protocol/bl_protocol.h"
#include "cdefs/cdefs.h"
#include "../flash/bl_flash_config.h"


#define BL_DEVICE_ID_SIZE    (12U)
#define BL_DEVICE_UID_BASE   (0x1FFF7590UL)


typedef struct
{
    uint32_t size;
    uint32_t offset;
    uint32_t crc;
    uint32_t start_address;

} BL_FirmwareInfo_t;

typedef struct
{
    BL_Protocol_Packet_t  last_packet;
    BL_State_t            last_state;
    bool                  last_packet_valid;

} BL_RetryContext_t;


static BL_RetryContext_t bl_retry_context =
{
    .last_packet       = {0},
    .last_state        = BL_STATE_WAIT_SYNC,
    .last_packet_valid = false
};


static BL_Protocol_Parser_t protocol_parser;
static BL_Protocol_Packet_t packet;
static BL_State_t bl_state = BL_STATE_WAIT_SYNC;
static BL_FirmwareInfo_t bl_firmware_info = {0};




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



static void Bootloader_SendDeviceID(void)
{
    BL_Protocol_Packet_t packet;
    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t frame_length;

    const volatile uint8_t *uid =
        (const volatile uint8_t *)BL_DEVICE_UID_BASE;

    if (!BL_Protocol_CreateCommandDataPacket(
            &packet,
            BL_CMD_DEVICE_ID_RES,
            (const uint8_t *)uid,
            BL_DEVICE_ID_SIZE))
    {
        Bootloader_SendNACK();
        return;
    }

    if (BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &frame_length))
    {
        BL_Transport_Write(buffer, frame_length);
    }
    else
    {
        Bootloader_SendNACK();
    }
}


static void Bootloader_SendCommandPacket(
    BL_Command_t command
)
{
    BL_Protocol_Packet_t packet;
    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t length;

    BL_Protocol_CreateCommandPacket(
        &packet,
        command
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


    /* --------------------------------------------------------------
     * Extract command
     * -------------------------------------------------------------- */
    if (!BL_Protocol_ExtractCommand(
            packet,
            &command))
    {
        Bootloader_SendNACK();
        return;
    }


    /* --------------------------------------------------------------
     * Save last packet and state
     *
     * RETX must not overwrite the previous packet.
     * -------------------------------------------------------------- */
    if (command != BL_CMD_RETX)
    {
        bl_retry_context.last_packet = *packet;
        bl_retry_context.last_state = bl_state;
        bl_retry_context.last_packet_valid = true;
    }


    /* --------------------------------------------------------------
     * Command processing
     * -------------------------------------------------------------- */
    switch (command)
    {
        /* ----------------------------------------------------------
         * SYNC
         * ---------------------------------------------------------- */
        case BL_CMD_SYNC_OBSERVED:
        {
            if (bl_state == BL_STATE_WAIT_SYNC)
            {
                bl_state = BL_STATE_CONNECTED;

                Bootloader_SendACK();
            }
            else
            {
                Bootloader_SendNACK();

                bl_state = BL_STATE_WAIT_SYNC;
            }

            break;
        }


        /* ----------------------------------------------------------
         * Firmware Update Request
         * ---------------------------------------------------------- */
        case BL_CMD_FW_UPDATE_REQ:
        {
            /* Handle firmware update request */

            break;
        }


        /* ----------------------------------------------------------
         * Device ID Request
         * ---------------------------------------------------------- */
        case BL_CMD_DEVICE_ID_REQ:
        {
            if (bl_state == BL_STATE_CONNECTED)
            {
                Bootloader_SendDeviceID();

                bl_state = BL_STATE_WAIT_FW_LENGTH;
            }
            else
            {
                Bootloader_SendNACK();

                bl_state = BL_STATE_WAIT_SYNC;
            }

            break;
        }


        /* ----------------------------------------------------------
         * Firmware Size
         * ---------------------------------------------------------- */
        case BL_CMD_FW_SIZE:
        {
            uint8_t  data[4];
            uint8_t  data_length;
            uint32_t firmware_size;


            if (bl_state != BL_STATE_WAIT_FW_LENGTH)
            {
                Bootloader_SendNACK();

                bl_state = BL_STATE_WAIT_SYNC;

                break;
            }


            if (!BL_Protocol_ExtractData(
                    packet,
                    data,
                    &data_length))
            {
                Bootloader_SendRETX();

                break;
            }


            if (data_length != sizeof(uint32_t))
            {
                Bootloader_SendRETX();

                break;
            }


            firmware_size =
                ((uint32_t)data[0])        |
                ((uint32_t)data[1] << 8U)  |
                ((uint32_t)data[2] << 16U) |
                ((uint32_t)data[3] << 24U);


            if ((firmware_size == 0U) ||
                (firmware_size > BL_APP_SIZE))
            {
                Bootloader_SendCommandPacket(
                    BL_CMD_FW_OVER_SIZE
                );

                bl_state = BL_STATE_WAIT_SYNC;

                break;
            }


            /* Firmware size successfully received */
            bl_firmware_info.size = firmware_size;


            Bootloader_SendACK();


            bl_state = BL_STATE_WAIT_FW_START_ADDRESS;


            break;
        }


        /* ----------------------------------------------------------
         * Set Application Start Address
         * ---------------------------------------------------------- */
        case BL_CMD_SET_APP_START_ADDRESS:
        {
            uint8_t  data[4];
            uint8_t  data_length;
            uint32_t start_address;


            if (bl_state != BL_STATE_WAIT_FW_START_ADDRESS)
            {
                Bootloader_SendNACK();

                bl_state = BL_STATE_WAIT_SYNC;

                break;
            }


            if (!BL_Protocol_ExtractData(
                    packet,
                    data,
                    &data_length))
            {
                Bootloader_SendRETX();

                break;
            }


            if (data_length != sizeof(uint32_t))
            {
                Bootloader_SendRETX();

                break;
            }


            start_address =
                ((uint32_t)data[0])        |
                ((uint32_t)data[1] << 8U)  |
                ((uint32_t)data[2] << 16U) |
                ((uint32_t)data[3] << 24U);


            if ((start_address < BL_APP_START_ADDRESS) ||
                (start_address >
                 (BL_APP_START_ADDRESS + BL_APP_SIZE)))
            {
                Bootloader_SendCommandPacket(
                    BL_CMD_APP_START_ADDRESS_ERROR
                );

                bl_state = BL_STATE_WAIT_SYNC;

                break;
            }


            /* Start address successfully received */
            bl_firmware_info.start_address = start_address;

            bl_firmware_info.offset =
                start_address - BL_APP_START_ADDRESS;


            Bootloader_SendACK();


            bl_state = BL_STATE_WAIT_SYNC;


            break;
        }


        /* ----------------------------------------------------------
         * RETX
         *
         * Re-process the previously received packet using
         * the state in which that packet was originally received.
         * ---------------------------------------------------------- */
        case BL_CMD_RETX:
        {
            BL_Protocol_Packet_t retry_packet;


            if (!bl_retry_context.last_packet_valid)
            {
                Bootloader_SendNACK();
                bl_state = BL_STATE_WAIT_SYNC;
                break;
            }


            /*
             * Restore the state from before the previous
             * packet was processed.
             */
            bl_state = bl_retry_context.last_state;


            /*
             * Make a local copy.
             *
             * This prevents the stored packet from being
             * modified during RETX processing.
             */
            retry_packet = bl_retry_context.last_packet;


            /*
             * Re-process the previous packet.
             */
            Bootloader_ProcessPacket(&retry_packet);


            break;
        }


        /* ----------------------------------------------------------
         * Unknown Command
         * ---------------------------------------------------------- */
        default:
        {
            Bootloader_SendNACK();

            break;
        }
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