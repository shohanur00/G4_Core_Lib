#include "bl_metadata.h"

#include "../flash/bl_flash.h"
#include "../config/bl_flash_config.h"
#include "cdefs/cdefs.h"

bool BL_Metadata_Save(
    const BL_FirmwareMetadata_t *metadata
)
{
    if (metadata == NULL)
    {
        return false;
    }

    if (!BL_Flash_Erase_MetaData(
            BL_METADATA_START_ADDRESS,
            BL_METADATA_SIZE))
    {
        return false;
    }

    if (!BL_Flash_Write_MetaData(
            BL_METADATA_START_ADDRESS,
            (const uint8_t *)metadata,
            sizeof(BL_FirmwareMetadata_t)))
    {
        return false;
    }

    return BL_Flash_Verify_MetaData(
        BL_METADATA_START_ADDRESS,
        (const uint8_t *)metadata,
        sizeof(BL_FirmwareMetadata_t)
    );
}


bool BL_Metadata_Read(
    BL_FirmwareMetadata_t *metadata
)
{
    if (metadata == NULL)
    {
        return false;
    }

    return BL_Flash_Read_MetaData(
        BL_METADATA_START_ADDRESS,
        (uint8_t *)metadata,
        sizeof(BL_FirmwareMetadata_t)
    );
}


bool BL_Metadata_IsValid(
    const BL_FirmwareMetadata_t *metadata
)
{
    if (metadata == NULL)
    {
        return false;
    }

    if (metadata->magic != BL_FIRMWARE_METADATA_MAGIC)
    {
        return false;
    }

    if (metadata->start_address < BL_APP_START_ADDRESS)
    {
        return false;
    }

    if (metadata->start_address >= BL_APP_END_ADDRESS)
    {
        return false;
    }

    if (metadata->size == 0U)
    {
        return false;
    }

    if (metadata->size >
        (BL_APP_END_ADDRESS - metadata->start_address))
    {
        return false;
    }

    return true;
}