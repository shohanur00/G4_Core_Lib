#ifndef BL_FLASH_H
#define BL_FLASH_H

#include <stdint.h>
#include <stdbool.h>

#include "../config/bl_flash_config.h"


bool BL_Flash_Init(void);

bool BL_Flash_Erase(
    uint32_t address,
    uint32_t length
);

bool BL_Flash_Write(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
);

bool BL_Flash_Verify(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
);

bool BL_Flash_IsErased(
    uint32_t address,
    uint32_t length
);


bool BL_Flash_Read(
    uint32_t address,
    uint8_t *data,
    uint32_t length
);


uint16_t BL_Flash_CalculateCRC(
    uint32_t address,
    uint32_t length
);

#endif /* BL_FLASH_H */