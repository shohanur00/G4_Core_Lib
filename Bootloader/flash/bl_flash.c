#include "bl_flash.h"
#include "stm32g431xx.h"

#include <stddef.h>



/* ============================================================================
 * Flash Constants
 * ========================================================================== */

#define BL_FLASH_PAGE_SIZE          (2048UL)
#define BL_FLASH_DOUBLE_WORD_SIZE   (8UL)

#define BL_FLASH_KEY1               (0x45670123UL)
#define BL_FLASH_KEY2               (0xCDEF89ABUL)


/* ============================================================================
 * Flash Status Flags
 * ========================================================================== */

#define BL_FLASH_ERROR_FLAGS \
    (FLASH_SR_OPERR   | \
     FLASH_SR_PROGERR | \
     FLASH_SR_WRPERR  | \
     FLASH_SR_PGAERR  | \
     FLASH_SR_SIZERR  | \
     FLASH_SR_PGSERR  | \
     FLASH_SR_MISERR  | \
     FLASH_SR_FASTERR | \
     FLASH_SR_RDERR   | \
     FLASH_SR_OPTVERR)

#define BL_FLASH_STATUS_FLAGS \
    (FLASH_SR_EOP | BL_FLASH_ERROR_FLAGS)


/* ============================================================================
 * Private Functions
 * ========================================================================== */

static bool BL_Flash_IsAddressValid(
    uint32_t address
)
{
    if (address < BL_APP_START_ADDRESS)
    {
        return false;
    }

    if (address >=
        (BL_APP_START_ADDRESS + BL_APP_SIZE))
    {
        return false;
    }

    return true;
}


static bool BL_Flash_IsAddressValid_MetaData(
    uint32_t address
)
{
    if (address < BL_METADATA_START_ADDRESS)
    {
        return false;
    }

    if (address >=
        (BL_METADATA_START_ADDRESS + BL_METADATA_SIZE))
    {
        return false;
    }

    return true;
}

static bool BL_Flash_IsRangeValid(
    uint32_t address,
    uint32_t length
)
{
    uint32_t end_address;

    if (length == 0U)
    {
        return false;
    }

    if (!BL_Flash_IsAddressValid(address))
    {
        return false;
    }

    end_address = address + length - 1U;

    /* Overflow check */
    if (end_address < address)
    {
        return false;
    }

    if (end_address >=
        (BL_APP_START_ADDRESS + BL_APP_SIZE))
    {
        return false;
    }

    return true;
}



static bool BL_Flash_IsRangeValid_MetaData(
    uint32_t address,
    uint32_t length
)
{
    uint32_t end_address;

    if (length == 0U)
    {
        return false;
    }

    if (!BL_Flash_IsAddressValid_MetaData(address))
    {
        return false;
    }

    end_address = address + length - 1U;

    /* Overflow check */
    if (end_address < address)
    {
        return false;
    }

    if (end_address >=
        (BL_METADATA_START_ADDRESS + BL_METADATA_SIZE))
    {
        return false;
    }

    return true;
}


#define BL_FLASH_TIMEOUT_LOOPS  4000000UL   /* tune: worst-case page erase er cheye boro */

static bool BL_Flash_WaitWhileBusy(void)
{
    uint32_t timeout = BL_FLASH_TIMEOUT_LOOPS;

    while ((FLASH->SR & FLASH_SR_BSY) != 0U)
    {
        if (timeout-- == 0U)
        {
            return false;
        }
    }

    return true;
}


static void BL_Flash_ClearStatusFlags(void)
{
    FLASH->SR = BL_FLASH_STATUS_FLAGS;
}


static void BL_Flash_Unlock(void)
{
    if ((FLASH->CR & FLASH_CR_LOCK) != 0U)
    {
        FLASH->KEYR = BL_FLASH_KEY1;
        FLASH->KEYR = BL_FLASH_KEY2;
    }
}


static void BL_Flash_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}


static bool BL_Flash_HasError(void)
{
    return ((FLASH->SR & BL_FLASH_ERROR_FLAGS) != 0U);
}


/* ============================================================================
 * Initialization
 * ========================================================================== */

bool BL_Flash_Init(void)
{
    BL_Flash_Unlock();

    if ((FLASH->CR & FLASH_CR_LOCK) != 0U)
    {
        return false;
    }

    return true;
}


/* ============================================================================
 * Flash Erase
 * ========================================================================== */

bool BL_Flash_Erase(
    uint32_t address,
    uint32_t length
)
{
    uint32_t start_page;
    uint32_t end_page;
    uint32_t page;

    if (!BL_Flash_IsRangeValid(address, length))
    {
        return false;
    }

    start_page =
        (address - BL_FLASH_START_ADDRESS) /
        BL_FLASH_PAGE_SIZE;

    end_page =
        ((address + length - 1U) -
         BL_FLASH_START_ADDRESS) /
        BL_FLASH_PAGE_SIZE;


    BL_Flash_Unlock();

    if ((FLASH->CR & FLASH_CR_LOCK) != 0U)
    {
        return false;
    }

    for (page = start_page;
         page <= end_page;
         page++)
    {

        BL_Flash_WaitWhileBusy();

        BL_Flash_ClearStatusFlags();

        FLASH->CR &= ~FLASH_CR_PNB;

        FLASH->CR |=
            ((page << FLASH_CR_PNB_Pos) &
             FLASH_CR_PNB);

        FLASH->CR |= FLASH_CR_PER;

        FLASH->CR |= FLASH_CR_STRT;

        BL_Flash_WaitWhileBusy();

        FLASH->CR &= ~FLASH_CR_PER;

        if (BL_Flash_HasError())
        {
            BL_Flash_Lock();

            return false;
        }

    }

    BL_Flash_Lock();

    return true;
}


bool BL_Flash_Erase_MetaData(
    uint32_t address,
    uint32_t length
)
{
    uint32_t start_page;
    uint32_t end_page;
    uint32_t page;

    if (!BL_Flash_IsRangeValid_MetaData(address, length))
    {
        return false;
    }

    start_page =
        (address - BL_FLASH_START_ADDRESS) /
        BL_FLASH_PAGE_SIZE;

    end_page =
        ((address + length - 1U) -
         BL_FLASH_START_ADDRESS) /
        BL_FLASH_PAGE_SIZE;


    BL_Flash_Unlock();

    if ((FLASH->CR & FLASH_CR_LOCK) != 0U)
    {
        return false;
    }

    for (page = start_page;
         page <= end_page;
         page++)
    {

        BL_Flash_WaitWhileBusy();

        BL_Flash_ClearStatusFlags();

        FLASH->CR &= ~FLASH_CR_PNB;

        FLASH->CR |=
            ((page << FLASH_CR_PNB_Pos) &
             FLASH_CR_PNB);

        FLASH->CR |= FLASH_CR_PER;

        FLASH->CR |= FLASH_CR_STRT;

        BL_Flash_WaitWhileBusy();

        FLASH->CR &= ~FLASH_CR_PER;

        if (BL_Flash_HasError())
        {
            BL_Flash_Lock();

            return false;
        }

    }

    BL_Flash_Lock();

    return true;
}

/* ============================================================================
 * Flash Write
 * ========================================================================== */

bool BL_Flash_Write(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
)
{
    uint32_t index;
    uint64_t double_word;

    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    if (!BL_Flash_IsRangeValid(address, length))
    {
        return false;
    }

    /*
     * STM32G4 double-word programming requires
     * 64-bit aligned address and 8-byte data.
     */
    if ((address & (BL_FLASH_DOUBLE_WORD_SIZE - 1U)) != 0U)
    {
        return false;
    }

    if ((length & (BL_FLASH_DOUBLE_WORD_SIZE - 1U)) != 0U)
    {
        return false;
    }

    BL_Flash_Unlock();

    if ((FLASH->CR & FLASH_CR_LOCK) != 0U)
    {
        return false;
    }

    for (index = 0U;
         index < length;
         index += BL_FLASH_DOUBLE_WORD_SIZE)
    {
        /*
         * Wait until previous operation is finished.
         */
        BL_Flash_WaitWhileBusy();

        /*
         * Clear previous status flags.
         */
        BL_Flash_ClearStatusFlags();

        /*
         * Build 64-bit double word.
         */
        double_word =
            ((uint64_t)data[index + 0U] <<  0U) |
            ((uint64_t)data[index + 1U] <<  8U) |
            ((uint64_t)data[index + 2U] << 16U) |
            ((uint64_t)data[index + 3U] << 24U) |
            ((uint64_t)data[index + 4U] << 32U) |
            ((uint64_t)data[index + 5U] << 40U) |
            ((uint64_t)data[index + 6U] << 48U) |
            ((uint64_t)data[index + 7U] << 56U);

        /*
         * Enable programming.
         */
        FLASH->CR |= FLASH_CR_PG;

        /*
         * STM32G4 double-word programming:
         * first 32-bit write followed by second 32-bit write.
         */
        *(volatile uint32_t *)address =
            (uint32_t)(double_word & 0xFFFFFFFFUL);

        *(volatile uint32_t *)(address + 4U) =
            (uint32_t)(double_word >> 32U);

        /*
         * Wait until programming completes.
         */
        BL_Flash_WaitWhileBusy();

        /*
         * Disable programming.
         */
        FLASH->CR &= ~FLASH_CR_PG;

        /*
         * Check programming errors.
         */
        if (BL_Flash_HasError())
        {
            BL_Flash_Lock();
            return false;
        }

        /*
         * Verify programmed double word.
         */
        if (*(volatile uint64_t *)address != double_word)
        {
            BL_Flash_Lock();
            return false;
        }

        address += BL_FLASH_DOUBLE_WORD_SIZE;
    }

    BL_Flash_Lock();

    return true;
}



bool BL_Flash_Write_MetaData(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
)
{
    uint32_t index;
    uint64_t double_word;

    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    if (!BL_Flash_IsRangeValid_MetaData(address, length))
    {
        return false;
    }

    /*
     * STM32G4 double-word programming requires
     * 64-bit aligned address and 8-byte data.
     */
    if ((address & (BL_FLASH_DOUBLE_WORD_SIZE - 1U)) != 0U)
    {
        return false;
    }

    if ((length & (BL_FLASH_DOUBLE_WORD_SIZE - 1U)) != 0U)
    {
        return false;
    }

    BL_Flash_Unlock();

    if ((FLASH->CR & FLASH_CR_LOCK) != 0U)
    {
        return false;
    }

    for (index = 0U;
         index < length;
         index += BL_FLASH_DOUBLE_WORD_SIZE)
    {
        /*
         * Wait until previous operation is finished.
         */
        BL_Flash_WaitWhileBusy();

        /*
         * Clear previous status flags.
         */
        BL_Flash_ClearStatusFlags();

        /*
         * Build 64-bit double word.
         */
        double_word =
            ((uint64_t)data[index + 0U] <<  0U) |
            ((uint64_t)data[index + 1U] <<  8U) |
            ((uint64_t)data[index + 2U] << 16U) |
            ((uint64_t)data[index + 3U] << 24U) |
            ((uint64_t)data[index + 4U] << 32U) |
            ((uint64_t)data[index + 5U] << 40U) |
            ((uint64_t)data[index + 6U] << 48U) |
            ((uint64_t)data[index + 7U] << 56U);

        /*
         * Enable programming.
         */
        FLASH->CR |= FLASH_CR_PG;

        /*
         * STM32G4 double-word programming:
         * first 32-bit write followed by second 32-bit write.
         */
        *(volatile uint32_t *)address =
            (uint32_t)(double_word & 0xFFFFFFFFUL);

        *(volatile uint32_t *)(address + 4U) =
            (uint32_t)(double_word >> 32U);

        /*
         * Wait until programming completes.
         */
        BL_Flash_WaitWhileBusy();

        /*
         * Disable programming.
         */
        FLASH->CR &= ~FLASH_CR_PG;

        /*
         * Check programming errors.
         */
        if (BL_Flash_HasError())
        {
            BL_Flash_Lock();
            return false;
        }

        /*
         * Verify programmed double word.
         */
        if (*(volatile uint64_t *)address != double_word)
        {
            BL_Flash_Lock();
            return false;
        }

        address += BL_FLASH_DOUBLE_WORD_SIZE;
    }

    BL_Flash_Lock();

    return true;
}

/* ============================================================================
 * Flash Verify
 * ========================================================================== */

bool BL_Flash_Verify(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
)
{
    uint32_t index;

    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    if (!BL_Flash_IsRangeValid(address, length))
    {
        return false;
    }

    for (index = 0U;
         index < length;
         index++)
    {
        if (*(volatile uint8_t *)(address + index) != data[index])
        {
            return false;
        }
    }

    return true;
}



bool BL_Flash_Verify_MetaData(
    uint32_t       address,
    const uint8_t *data,
    uint32_t       length
)
{
    uint32_t index;

    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    if (!BL_Flash_IsRangeValid_MetaData(address, length))
    {
        return false;
    }

    for (index = 0U;
         index < length;
         index++)
    {
        if (*(volatile uint8_t *)(address + index) != data[index])
        {
            return false;
        }
    }

    return true;
}


/* ============================================================================
 * Check Flash Erased
 * ========================================================================== */

bool BL_Flash_IsErased(
    uint32_t address,
    uint32_t length
)
{
    uint32_t index;

    if (length == 0U)
    {
        return false;
    }

    if (!BL_Flash_IsRangeValid(address, length))
    {
        return false;
    }

    for (index = 0U;
         index < length;
         index++)
    {
        if (*(volatile uint8_t *)(address + index) != 0xFFU)
        {
            return false;
        }
    }

    return true;
}


bool BL_Flash_IsErased_MetaData(
    uint32_t address,
    uint32_t length
)
{
    uint32_t index;

    if (length == 0U)
    {
        return false;
    }

    if (!BL_Flash_IsRangeValid_MetaData(address, length))
    {
        return false;
    }

    for (index = 0U;
         index < length;
         index++)
    {
        if (*(volatile uint8_t *)(address + index) != 0xFFU)
        {
            return false;
        }
    }

    return true;
}


bool BL_Flash_Read(
    uint32_t address,
    uint8_t *data,
    uint32_t length
)
{
    uint32_t index;

    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    if (!BL_Flash_IsRangeValid(address, length))
    {
        return false;
    }

    for (index = 0U; index < length; index++)
    {
        data[index] =
            *(volatile const uint8_t *)(address + index);
    }

    return true;
}



bool BL_Flash_Read_MetaData(
    uint32_t address,
    uint8_t *data,
    uint32_t length
)
{
    uint32_t index;

    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    if (!BL_Flash_IsRangeValid_MetaData(address, length))
    {
        return false;
    }

    for (index = 0U; index < length; index++)
    {
        data[index] =
            *(volatile const uint8_t *)(address + index);
    }

    return true;
}



uint16_t BL_Flash_CalculateCRC(
    uint32_t address,
    uint32_t length
)
{
    uint16_t crc = BL_FLASH_CRC16_INITIAL;
    uint8_t  data;
    uint8_t  bit;

    if (length == 0U)
    {
        return crc;
    }

    if (!BL_Flash_IsRangeValid(address, length))
    {
        return 0U;
    }

    while (length > 0U)
    {
        /*
         * Read one byte directly from Flash.
         */
        data = *(volatile uint8_t *)address;

        /*
         * CRC-16-CCITT-FALSE
         */
        crc ^= ((uint16_t)data << 8U);

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x8000U) != 0U)
            {
                crc =
                    (uint16_t)((crc << 1U) ^
                               BL_FLASH_CRC16_POLYNOMIAL);
            }
            else
            {
                crc = (uint16_t)(crc << 1U);
            }
        }

        address++;
        length--;
    }

    return crc;
}