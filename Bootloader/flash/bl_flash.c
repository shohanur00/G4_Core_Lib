#include "bl_flash.h"

#include "stm32g431xx.h"
#include "cdefs/cdefs.h"


/* ============================================================================
 * Flash Unlock Keys
 * ========================================================================== */

#define BL_FLASH_KEY1    (0x45670123UL)
#define BL_FLASH_KEY2    (0xCDEF89ABUL)


/* ============================================================================
 * Internal Helpers
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


/* ============================================================================
 * Flash Initialization
 * ========================================================================== */

bool BL_Flash_Init(void)
{
    /*
     * Flash peripheral does not require a clock enable.
     *
     * Make sure Flash is locked before normal operation.
     */

    if ((FLASH->CR & FLASH_CR_LOCK) == 0U)
    {
        FLASH->KEYR = BL_FLASH_KEY1;
        FLASH->KEYR = BL_FLASH_KEY2;
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

    if (!BL_Flash_IsRangeValid(
            address,
            length))
    {
        return false;
    }

    /*
     * STM32G4 Flash page size = 2 KB
     */

    start_page =
        (address - BL_FLASH_START_ADDRESS) /
        2048U;

    end_page =
        ((address + length - 1U) -
         BL_FLASH_START_ADDRESS) /
        2048U;


    /*
     * Unlock Flash
     */

    if ((FLASH->CR & FLASH_CR_LOCK) != 0U)
    {
        FLASH->KEYR = BL_FLASH_KEY1;
        FLASH->KEYR = BL_FLASH_KEY2;
    }


    /*
     * Clear previous status flags
     */

    FLASH->SR = FLASH_SR_ALL_ERRORS;


    for (page = start_page;
         page <= end_page;
         page++)
    {
        /*
         * Wait until Flash is not busy
         */

        while ((FLASH->SR & FLASH_SR_BSY) != 0U)
        {
        }


        /*
         * Page erase
         */

        FLASH->CR &= ~FLASH_CR_PNB;

        FLASH->CR |=
            ((page << FLASH_CR_PNB_Pos) &
             FLASH_CR_PNB);

        FLASH->CR |= FLASH_CR_PER;
        FLASH->CR |= FLASH_CR_STRT;


        /*
         * Wait for erase completion
         */

        while ((FLASH->SR & FLASH_SR_BSY) != 0U)
        {
        }


        /*
         * Check erase error
         */

        if ((FLASH->SR & FLASH_SR_ALL_ERRORS) != 0U)
        {
            FLASH->CR &= ~FLASH_CR_PER;

            FLASH->CR |= FLASH_CR_LOCK;

            return false;
        }

        FLASH->CR &= ~FLASH_CR_PER;
    }


    /*
     * Lock Flash again
     */

    FLASH->CR |= FLASH_CR_LOCK;

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


    if ((data == NULL) ||
        (length == 0U))
    {
        return false;
    }


    if (!BL_Flash_IsRangeValid(
            address,
            length))
    {
        return false;
    }


    /*
     * STM32G4 programs Flash using 64-bit double words.
     *
     * Address must therefore be 8-byte aligned.
     */

    if ((address & 0x07U) != 0U)
    {
        return false;
    }


    /*
     * Length must be a multiple of 8 bytes.
     */

    if ((length & 0x07U) != 0U)
    {
        return false;
    }


    /*
     * Unlock Flash
     */

    if ((FLASH->CR & FLASH_CR_LOCK) != 0U)
    {
        FLASH->KEYR = BL_FLASH_KEY1;
        FLASH->KEYR = BL_FLASH_KEY2;
    }


    /*
     * Clear previous status flags
     */

    FLASH->SR = FLASH_SR_ALL_ERRORS;


    for (index = 0U;
         index < length;
         index += 8U)
    {
        /*
         * Wait until Flash is ready
         */

        while ((FLASH->SR & FLASH_SR_BSY) != 0U)
        {
        }


        /*
         * Prepare 64-bit data
         */

        double_word =
            ((uint64_t)data[index + 0U] << 0U)  |
            ((uint64_t)data[index + 1U] << 8U)  |
            ((uint64_t)data[index + 2U] << 16U) |
            ((uint64_t)data[index + 3U] << 24U) |
            ((uint64_t)data[index + 4U] << 32U) |
            ((uint64_t)data[index + 5U] << 40U) |
            ((uint64_t)data[index + 6U] << 48U) |
            ((uint64_t)data[index + 7U] << 56U);


        /*
         * Enable programming
         */

        FLASH->CR |= FLASH_CR_PG;


        /*
         * Program 64-bit double word
         */

        *(volatile uint32_t *)address =
            (uint32_t)(double_word & 0xFFFFFFFFUL);

        *(volatile uint32_t *)(address + 4U) =
            (uint32_t)(double_word >> 32U);


        /*
         * Wait until programming completes
         */

        while ((FLASH->SR & FLASH_SR_BSY) != 0U)
        {
        }


        /*
         * Disable programming
         */

        FLASH->CR &= ~FLASH_CR_PG;


        /*
         * Check errors
         */

        if ((FLASH->SR & FLASH_SR_ALL_ERRORS) != 0U)
        {
            FLASH->CR |= FLASH_CR_LOCK;

            return false;
        }


        /*
         * Verify immediately
         */

        if (*(volatile uint64_t *)address !=
            double_word)
        {
            FLASH->CR |= FLASH_CR_LOCK;

            return false;
        }


        address += 8U;
    }


    /*
     * Lock Flash
     */

    FLASH->CR |= FLASH_CR_LOCK;

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


    if ((data == NULL) ||
        (length == 0U))
    {
        return false;
    }


    if (!BL_Flash_IsRangeValid(
            address,
            length))
    {
        return false;
    }


    for (index = 0U;
         index < length;
         index++)
    {
        if (*(volatile uint8_t *)(address + index) !=
            data[index])
        {
            return false;
        }
    }


    return true;
}


/* ============================================================================
 * Flash Is Erased
 * ========================================================================== */

bool BL_Flash_IsErased(
    uint32_t address,
    uint32_t length
)
{
    uint32_t index;


    if (!BL_Flash_IsRangeValid(
            address,
            length))
    {
        return false;
    }


    for (index = 0U;
         index < length;
         index++)
    {
        if (*(volatile uint8_t *)(address + index) !=
            0xFFU)
        {
            return false;
        }
    }


    return true;
}