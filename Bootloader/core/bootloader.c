#include "bootloader.h"

#include "../transport/bl_transport.h"
#include "cdefs/cdefs.h"
#include "../config/bl_config.h"
#include "../config/bl_flash_config.h"
#include "../flash/bl_flash.h"
#include "flash/bl_flash.h"
#include <stdint.h>
#include "../metadata/bl_metadata.h"


#define BL_MAX_RETRY_COUNT    (10U)


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

    uint8_t              retry_count;

    BL_FirmwareMetadata_t metadata;

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
    },

    .retry_count = 0,
    .metadata   = {0}
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

    bl_context.retry_count = 0U;
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


static bool Bootloader_WriteFirmwareData(
    uint32_t       address,
    const uint8_t *data,
    uint8_t        length
)
{
    uint8_t buffer[BL_PROTOCOL_MAX_DATA_SIZE];

    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    /*
     * Copy actual packet data
     */
    for (uint8_t i = 0U; i < length; i++)
    {
        buffer[i] = data[i];
    }

    /*
     * Pad remaining bytes with 0xFF
     */
    for (uint8_t i = length;
         i < BL_PROTOCOL_MAX_DATA_SIZE;
         i++)
    {
        buffer[i] = 0xFFU;
    }

    /*
     * Write complete 16-byte packet
     *
     * BL_Flash_Write() internally performs:
     * 8-byte + 8-byte programming.
     */
    if (!BL_Flash_Write(
            address,
            buffer,
            BL_PROTOCOL_MAX_DATA_SIZE))
    {
        return false;
    }

    /*
     * Verify complete 16-byte physical write
     */
    if (!BL_Flash_Verify(
            address,
            buffer,
            BL_PROTOCOL_MAX_DATA_SIZE))
    {
        return false;
    }

    return true;
}


static bool Bootloader_SendNACKAndCheckRetry(
    BL_ErrorCode_t error
)
{
    Bootloader_SendNACK(error);

    bl_context.retry_count++;

    if (bl_context.retry_count >= BL_MAX_RETRY_COUNT)
    {
        bl_context.retry_count = 0U;
        bl_context.retry.last_packet_valid = false;
        bl_context.state = BL_STATE_WAIT_SYNC;

        return false;
    }

    return true;
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
        Bootloader_SendNACKAndCheckRetry(
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

                bl_context.state = BL_STATE_WAIT_DEVICE_ID_REQ;

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
            if (bl_context.state == BL_STATE_WAIT_DEVICE_ID_REQ)
            {
                Bootloader_SendDeviceID();

                Bootloader_SaveRetryContext(
                    packet,
                    bl_context.state
                );

                bl_context.state = BL_STATE_WAIT_DEVICE_ID_CONFIRM;
            }
            else
            {
                Bootloader_SendNACK(BL_ERROR_STATE);
            }

            break;
        }

        case BL_CMD_DEVICE_ID_CONFIRM:
        {
            if (bl_context.state == BL_STATE_WAIT_DEVICE_ID_CONFIRM)
            {
                Bootloader_SendACK();

                bl_context.state = BL_STATE_CONNECTED;
            }
            else
            {
                Bootloader_SendNACK(BL_ERROR_STATE);
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
                BL_STATE_CONNECTED)
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
                Bootloader_SendNACKAndCheckRetry(
                    BL_ERROR_PROTOCOL
                );

                break;
            }


            if (data_length != sizeof(uint32_t))
            {
                Bootloader_SendNACKAndCheckRetry(
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
            bl_context.firmware_info.received_size = 0U;

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
                Bootloader_SendNACKAndCheckRetry(
                    BL_ERROR_PROTOCOL
                );

                break;
            }


            if (data_length != sizeof(uint32_t))
            {
                Bootloader_SendNACKAndCheckRetry(
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

            if (!BL_Flash_Erase_MetaData(BL_METADATA_START_ADDRESS,BL_METADATA_SIZE))
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_FLASH_ERASE);
                break;
            }

            
            if(!BL_Flash_IsErased_MetaData(BL_METADATA_START_ADDRESS, BL_METADATA_SIZE)){
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_FLASH_ERASE);
                break;
            }

            if(!BL_Flash_Erase(BL_APP_START_ADDRESS, BL_APP_SIZE)){
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_FLASH_ERASE);
                break;
            }
            
            if(!BL_Flash_IsErased(BL_APP_START_ADDRESS, BL_APP_SIZE)){
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_FLASH_ERASE);
                break;
            }

            Bootloader_SaveRetryContext(
                packet,
                bl_context.state
            );

            Bootloader_SendACK();


            bl_context.state = BL_STATE_PROGRAMMING;


            break;
        }

        case BL_CMD_FW_DATA:
        {
            uint8_t  data[BL_PROTOCOL_MAX_DATA_SIZE];
            uint8_t  data_length = 0U;
            uint32_t remaining;

            if (bl_context.state != BL_STATE_PROGRAMMING)
            {
                Bootloader_SendNACK(BL_ERROR_STATE);
                bl_context.state = BL_STATE_WAIT_SYNC;
                break;
            }

            if (!BL_Protocol_ExtractData(packet, data, &data_length))
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_PROTOCOL);
                break;
            }

            if (data_length == 0U)
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_DATA);
                break;
            }

            /*
            * Calculate remaining firmware bytes.
            */
            remaining =
                bl_context.firmware_info.size -
                bl_context.firmware_info.received_size;

            /*
            * Packet must not exceed remaining firmware size.
            */
            if ((uint32_t)data_length > remaining)
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_DATA);
                break;
            }

            /*
            * Write packet to Flash.
            */
            if (!Bootloader_WriteFirmwareData(
                    bl_context.firmware_info.write_address,
                    data,
                    data_length))
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_DATA);
                break;
            }

            /*
            * Update actual received firmware size.
            */
            bl_context.firmware_info.received_size += data_length;

            /*
            * Move physical Flash address by 16 bytes,
            * because Bootloader_WriteFirmwareData()
            * physically writes one complete 16-byte block.
            */
            bl_context.firmware_info.write_address +=
                BL_PROTOCOL_MAX_DATA_SIZE;

            /*
            * Check firmware transmission complete.
            */
            if (bl_context.firmware_info.received_size ==
                bl_context.firmware_info.size)
            {
                bl_context.state = BL_STATE_COMPLETE;

                Bootloader_SendCommandPacket(BL_CMD_UPDATE_SUCCESSFUL);

            }
            else
            {
                /*
                * More firmware data expected.
                */
                Bootloader_SendACK();
            }

            break;
        }

        case BL_CMD_CRC_CHECK:
        {
            uint16_t expected_crc;
            uint16_t calculated_crc;
            uint8_t  data[BL_PROTOCOL_MAX_DATA_SIZE];
            uint8_t  data_length = 0U;

            if (bl_context.state != BL_STATE_COMPLETE)
            {
                Bootloader_SendNACK(BL_ERROR_STATE);
                bl_context.state = BL_STATE_WAIT_SYNC;
                break;
            }

            if (!BL_Protocol_ExtractData(
                    packet,
                    data,
                    &data_length))
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_PROTOCOL);
                break;
            }

            /*
            * CRC16 = 2 bytes
            */
            if (data_length != 2U)
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_DATA);
                break;
            }

            /*
            * Extract expected CRC from PC
            */
            expected_crc =
                ((uint16_t)data[0] << 8U) |
                ((uint16_t)data[1]);

            /*
            * Calculate CRC from Flash
            */
            calculated_crc =
                BL_Flash_CalculateCRC(
                    bl_context.firmware_info.start_address,
                    bl_context.firmware_info.size
                );

            if (calculated_crc != expected_crc)
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_CRC);
                break;
            }

            bl_context.firmware_info.crc = calculated_crc;
            bl_context.metadata.magic = BL_FIRMWARE_METADATA_MAGIC;
            bl_context.metadata.start_address = bl_context.firmware_info.start_address;
            bl_context.metadata.size = bl_context.firmware_info.size;
            bl_context.metadata.crc = bl_context.firmware_info.crc;
            bl_context.metadata.update_status = BL_UPDATE_STATUS_VALID;

            if(!BL_Metadata_Save(&bl_context.metadata))
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_DATA);
            }
            /*
            * Firmware CRC verified successfully
            */
            Bootloader_SendACK();

            /*
            * Firmware is now valid
            */
            bl_context.state = BL_STATE_VALID;

            break;
        }

        /* ----------------------------------------------------------
        * Erase Application Firmware
        * ---------------------------------------------------------- */

        case BL_CMD_ERASE_FIRMWARE:
        {
            /*
            * Accept erase only after successful SYNC.
            */
            if (bl_context.state != BL_STATE_CONNECTED)
            {
                Bootloader_SendNACK(BL_ERROR_STATE);
                break;
            }

            /*
            * Invalidate old application metadata first.
            *
            * IMPORTANT:
            * Replace this call with your project's actual metadata
            * invalidation implementation. Do not erase an arbitrary
            * metadata range unless its layout is confirmed.
            */
            if (!BL_Flash_Erase_MetaData(BL_METADATA_START_ADDRESS,BL_METADATA_SIZE))
            {
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_FLASH_ERASE);
                break;
            }

            
            if(!BL_Flash_IsErased_MetaData(BL_METADATA_START_ADDRESS, BL_METADATA_SIZE)){
                Bootloader_SendNACKAndCheckRetry(BL_ERROR_FLASH_ERASE);
                break;
            }

            /*
            * Erase only the application flash region.
            * The configured region must exclude the bootloader
            * and any separately reserved metadata pages.
            */
            if (!BL_Flash_Erase(
                    BL_APP_START_ADDRESS,
                    BL_APP_SIZE))
            {
                Bootloader_SendNACKAndCheckRetry(
                    BL_ERROR_FLASH_ERASE
                );
                break;
            }

            /*
            * Verify the application region is erased.
            */
            if (!BL_Flash_IsErased(
                    BL_APP_START_ADDRESS,
                    BL_APP_SIZE))
            {
                Bootloader_SendNACKAndCheckRetry(
                    BL_ERROR_FLASH_ERASE
                );
                break;
            }

            /*
            * Erase successful.
            */
            Bootloader_SendACK();

            /*
            * Return to connected state so the host can request
            * device ID or start a new firmware update sequence.
            */
            bl_context.state = BL_STATE_CONNECTED;

            break;
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
            Bootloader_SendNACKAndCheckRetry(
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



BL_State_t Bootloader_GetState(void)
{
    return bl_context.state;
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

                    Bootloader_SendNACKAndCheckRetry(
                        BL_ERROR_CRC
                    );

                    break;


                case BL_PROTOCOL_PARSE_ERROR:

                    Bootloader_SendNACKAndCheckRetry(
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


bool Bootloader_ValidateApplication(void)
{
    
    uint16_t calculated_crc;

    /* Read firmware metadata from flash */
    if (!BL_Metadata_Read(&bl_context.metadata))
    {
        return false;
    }

    /* Validate metadata */
    if (!BL_Metadata_IsValid(&bl_context.metadata))
    {
        return false;
    }

    /* Check update status */
    if (bl_context.metadata.update_status != BL_UPDATE_STATUS_VALID)
    {
        return false;
    }

    /* Calculate CRC of the installed application */
    calculated_crc =
        BL_Flash_CalculateCRC(
            bl_context.metadata.start_address,
            bl_context.metadata.size
        );

    /* Compare calculated CRC with stored CRC */
    if (calculated_crc != bl_context.metadata.crc)
    {
        return false;
    }

    return true;
}


void Bootloader_GetBackTo_Idle(void)
{
    bl_context.state = BL_STATE_WAIT_SYNC;

    bl_context.retry_count = 0U;
    bl_context.retry.last_packet_valid = false;

    bl_context.firmware_info.size = 0U;
    bl_context.firmware_info.received_size = 0U;
    bl_context.firmware_info.start_address = 0U;
    bl_context.firmware_info.write_address = 0U;
    bl_context.firmware_info.offset = 0U;
    bl_context.firmware_info.crc = 0U;
}