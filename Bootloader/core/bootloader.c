#include "bootloader.h"

#include "../transport/bl_transport.h"
#include "cdefs/cdefs.h"
#include "../config/bl_config.h"
#include "../config/bl_flash_config.h"
#include "../flash/bl_flash.h"
#include "flash/bl_flash.h"
#include <stdint.h>





/* --------------------------------------------------------------------------
 * Bootloader Private Context
 * -------------------------------------------------------------------------- */

typedef struct
{
    BL_Protocol_Packet_t last_packet;
    BL_State_t           last_state;
    bool                 last_packet_valid;

} BL_RetryContext_t;


typedef struct
{
    BL_Protocol_Parser_t parser;

    BL_State_t           state;

    BL_FirmwareInfo_t    firmware_info;

    BL_RetryContext_t    retry;

} BL_Context_t;


/* --------------------------------------------------------------------------
 * Bootloader Context Instance
 * -------------------------------------------------------------------------- */

static BL_Context_t bl_context =
{
    .parser = {0},

    .state = BL_STATE_WAIT_SYNC,

    .firmware_info = {0},

    .retry =
    {
        .last_packet       = {0},
        .last_state        = BL_STATE_WAIT_SYNC,
        .last_packet_valid = false
    }
};


/* --------------------------------------------------------------------------
 * Private Functions
 * -------------------------------------------------------------------------- */

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


static void Bootloader_SendNACK(
    BL_ErrorCode_t error
)
{
    BL_Protocol_Packet_t packet;
    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t length;

    uint8_t error_data = (uint8_t)error;

    if (!BL_Protocol_CreateCommandDataPacket(
            &packet,
            BL_CMD_NACK,
            &error_data,
            sizeof(error_data)))
    {
        return;
    }

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
        Bootloader_SendNACK(BL_ERROR_PROTOCOL);
        return;
    }

    if (BL_Protocol_PacketToBytes(
            &packet,
            buffer,
            sizeof(buffer),
            &frame_length))
    {
        BL_Transport_Write(
            buffer,
            frame_length
        );
    }
    else
    {
        Bootloader_SendNACK(BL_ERROR_PROTOCOL);
    }
}


static void Bootloader_SendCommandPacket(
    BL_Command_t command
)
{
    BL_Protocol_Packet_t packet;
    uint8_t buffer[BL_PROTOCOL_MAX_FRAME_SIZE];
    uint16_t length;

    if (!BL_Protocol_CreateCommandPacket(
            &packet,
            command
        ))
    {
        Bootloader_SendNACK(BL_ERROR_PROTOCOL);
        return;
    }

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
    else
    {
        Bootloader_SendNACK(BL_ERROR_PROTOCOL);
    }
}


static void Bootloader_SaveRetryContext(
    const BL_Protocol_Packet_t *packet,
    BL_State_t state_before
)
{
    if (packet == NULL)
    {
        return;
    }

    bl_context.retry.last_packet = *packet;

    bl_context.retry.last_state = state_before;

    bl_context.retry.last_packet_valid = true;
}


/* --------------------------------------------------------------------------
 * Packet Processing
 * -------------------------------------------------------------------------- */

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
        Bootloader_SendNACK(
            BL_ERROR_COMMAND
        );

        return;
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
            if (bl_context.state == BL_STATE_WAIT_SYNC)
            {
                Bootloader_SaveRetryContext(
                    packet,
                    bl_context.state
                );

                bl_context.state = BL_STATE_CONNECTED;

                Bootloader_SendACK();
            }
            else
            {
                Bootloader_SendNACK(
                    BL_ERROR_STATE
                );

                bl_context.state = BL_STATE_WAIT_SYNC;
            }

            break;
        }


        /* ----------------------------------------------------------
         * Device ID Request
         * ---------------------------------------------------------- */

        case BL_CMD_DEVICE_ID_REQ:
        {
            if (bl_context.state == BL_STATE_CONNECTED)
            {
                Bootloader_SendDeviceID();

                Bootloader_SaveRetryContext(
                    packet,
                    bl_context.state
                );

                bl_context.state =
                    BL_STATE_WAIT_FW_LENGTH;
            }
            else
            {
                Bootloader_SendNACK(
                    BL_ERROR_STATE
                );

                bl_context.state =
                    BL_STATE_WAIT_SYNC;
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


            if (bl_context.state !=
                BL_STATE_WAIT_FW_LENGTH)
            {
                Bootloader_SendNACK(
                    BL_ERROR_STATE
                );

                bl_context.state =
                    BL_STATE_WAIT_SYNC;

                break;
            }


            if (!BL_Protocol_ExtractData(
                    packet,
                    data,
                    &data_length))
            {
                Bootloader_SendNACK(
                    BL_ERROR_PROTOCOL
                );

                break;
            }


            if (data_length != sizeof(uint32_t))
            {
                Bootloader_SendNACK(
                    BL_ERROR_LENGTH
                );

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
                Bootloader_SendNACK(
                    BL_ERROR_SIZE
                );

                bl_context.state =
                    BL_STATE_WAIT_SYNC;

                break;
            }


            /* Firmware size successfully received */

            bl_context.firmware_info.size =
                firmware_size;


            Bootloader_SaveRetryContext(
                packet,
                bl_context.state
            );

            Bootloader_SendACK();


            bl_context.state =
                BL_STATE_WAIT_FW_START_ADDRESS;


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


            if (bl_context.state !=
                BL_STATE_WAIT_FW_START_ADDRESS)
            {
                Bootloader_SendNACK(
                    BL_ERROR_STATE
                );

                bl_context.state =
                    BL_STATE_WAIT_SYNC;

                break;
            }


            if (!BL_Protocol_ExtractData(
                    packet,
                    data,
                    &data_length))
            {
                Bootloader_SendNACK(
                    BL_ERROR_PROTOCOL
                );

                break;
            }


            if (data_length != sizeof(uint32_t))
            {
                Bootloader_SendNACK(
                    BL_ERROR_LENGTH
                );

                break;
            }


            start_address =
                ((uint32_t)data[0])        |
                ((uint32_t)data[1] << 8U)  |
                ((uint32_t)data[2] << 16U) |
                ((uint32_t)data[3] << 24U);


            if ((start_address < BL_APP_START_ADDRESS) ||
                (start_address >= BL_APP_END_ADDRESS))
            {
                Bootloader_SendNACK(
                    BL_ERROR_ADDRESS
                );

                bl_context.state =
                    BL_STATE_WAIT_SYNC;

                break;
            }


            if (bl_context.firmware_info.size >
                (BL_APP_END_ADDRESS - start_address))
            {
                Bootloader_SendNACK(
                    BL_ERROR_SIZE
                );

                bl_context.state =
                    BL_STATE_WAIT_SYNC;

                break;
            }


            /* Start address successfully received */

            bl_context.firmware_info.start_address =
                start_address;

            bl_context.firmware_info.offset =
                start_address - BL_APP_START_ADDRESS;

            bl_context.firmware_info.write_address = bl_context.firmware_info.start_address;


            if(!BL_Flash_Erase(BL_APP_START_ADDRESS, BL_APP_SIZE)){
                Bootloader_SendNACK(BL_ERROR_FLASH_ERASE);
                break;
            }
            
            if(!BL_Flash_IsErased(BL_APP_START_ADDRESS, BL_APP_SIZE)){
                Bootloader_SendNACK(BL_ERROR_FLASH_ERASE);
                break;
            }

            Bootloader_SaveRetryContext(
                packet,
                bl_context.state
            );

            Bootloader_SendACK();


            bl_context.state = BL_STATE_READY;


            break;
        }

        case BL_CMD_FW_DATA:
        {
            if (bl_context.state != BL_STATE_READY){

                Bootloader_SendNACK(BL_ERROR_STATE);
                bl_context.state = BL_STATE_WAIT_SYNC;

                break;

            }

            uint8_t data[BL_PROTOCOL_MAX_DATA_SIZE];
            uint8_t data_length = 0;
            if (BL_Protocol_ExtractData(packet, data, &data_length) != BL_ERROR_NONE)
            {
                Bootloader_SendNACK(BL_ERROR_DATA);
                break;
                
            }


            bl_context.firmware_info.write_address += bl_context.firmware_info.offset;
            bl_context.firmware_info.offset = data_length;

        }

        /* ----------------------------------------------------------
         * RETX
         * ---------------------------------------------------------- */

        case BL_CMD_RETX:
        {
            BL_Protocol_Packet_t retry_packet;


            if (!bl_context.retry.last_packet_valid)
            {
                Bootloader_SendNACK(
                    BL_ERROR_NO_RETRY_PACKET
                );

                bl_context.state =
                    BL_STATE_WAIT_SYNC;

                break;
            }


            /*
             * Restore the state from before the previous
             * packet was processed.
             */

            bl_context.state =
                bl_context.retry.last_state;


            /*
             * Make a local copy so that the stored packet
             * remains unchanged.
             */

            retry_packet =
                bl_context.retry.last_packet;


            /*
             * Re-process the previous valid packet.
             */

            Bootloader_ProcessPacket(
                &retry_packet
            );

            break;
        }


        /* ----------------------------------------------------------
         * Unknown Command
         * ---------------------------------------------------------- */

        default:
        {
            Bootloader_SendNACK(
                BL_ERROR_COMMAND
            );

            break;
        }
    }
}


/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

void Bootloader_Init(void)
{
    BL_Protocol_Parser_Init(
        &bl_context.parser
    );
    BL_Flash_Init();
    BL_Transport_Init();
}


void Bootloader_Process(void)
{
    uint8_t byte;

    while (BL_Transport_DataAvailable() > 0U)
    {
        if (BL_Transport_ReadByte(&byte))
        {
            BL_Protocol_ParseResult_t result =
                BL_Protocol_Parser_PushByte(
                    &bl_context.parser,
                    byte
                );


            switch (result)
            {
                case BL_PROTOCOL_PARSE_PACKET_READY:

                    Bootloader_ProcessPacket(
                        &bl_context.parser.packet
                    );

                    break;


                case BL_PROTOCOL_PARSE_CRC_ERROR:

                    Bootloader_SendNACK(
                        BL_ERROR_CRC
                    );

                    break;


                case BL_PROTOCOL_PARSE_ERROR:

                    Bootloader_SendNACK(
                        BL_ERROR_PROTOCOL
                    );

                    break;


                case BL_PROTOCOL_PARSE_IN_PROGRESS:

                    /* Wait for more bytes */

                    break;


                default:

                    break;
            }
        }
    }
}