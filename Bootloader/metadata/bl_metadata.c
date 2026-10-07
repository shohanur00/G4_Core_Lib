#include "bl_metadata.h"

#include "../flash/bl_flash.h"
#include "../config/bl_flash_config.h"

bool BL_Metadata_Save(
    const BL_FirmwareMetadata_t *metadata
)
{
    if (metadata == NULL)
    {
        return false;
    }

    if (!BL_Flash_Erase(
            BL_METADATA_START_ADDRESS,
            BL_METADATA_SIZE))
    {
        return false;
    }

    if (!BL_Flash_Write(
            BL_METADATA_START_ADDRESS,
            (const uint8_t *)metadata,
            sizeof(BL_FirmwareMetadata_t)))
    {
        return false;
    }

    return BL_Flash_Verify(
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

    return BL_Flash_Read(
        BL_METADATA_START_ADDRESS,
        (uint8_t *)metadata,
        sizeof(BL_FirmwareMetadata_t)
    );
}