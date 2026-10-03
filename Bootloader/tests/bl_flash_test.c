#include "bl_flash_test.h"
#include "../flash/bl_flash.h"
#include "logger/frontend/logger.h"

#include <stdint.h>
#include <stdbool.h>


/* ============================================================================
 * Flash Test Configuration
 * ========================================================================== */

#define BL_FLASH_TEST_ADDRESS     (BL_APP_START_ADDRESS)
#define BL_FLASH_TEST_LENGTH      (16U)


/* ============================================================================
 * Test Data
 * ========================================================================== */

static const uint8_t flash_test_data[BL_FLASH_TEST_LENGTH] =
{
    0x11U, 0x22U, 0x33U, 0x44U,
    0x55U, 0x66U, 0x77U, 0x88U,
    0xA1U, 0xB2U, 0xC3U, 0xD4U,
    0xE5U, 0xF6U, 0x12U, 0x34U
};


/* ============================================================================
 * Private Functions
 * ========================================================================== */

static void BL_Flash_Test_PrintData(
    const uint8_t *data,
    uint32_t length
)
{
    uint32_t index;

    for (index = 0U; index < length; index++)
    {
        LOG_DEBUG(
            LOG_MODULE_BOOTLOADER,
            "[FLASH TEST] data[%u] = 0x%X",
            index,
            data[index]
        );
    }
}


/* ============================================================================
 * Flash Test
 * ========================================================================== */

void BL_Flash_Test(void)
{
    bool result;


    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "========================================"
    );

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "FLASH TEST STARTED"
    );

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "Address : 0x%X",
        BL_FLASH_TEST_ADDRESS
    );

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "Length  : %u bytes",
        BL_FLASH_TEST_LENGTH
    );

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "========================================"
    );


    /* ------------------------------------------------------------------------
     * Step 1: Initialize
     * ---------------------------------------------------------------------- */

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[1] Flash initialization"
    );

    result = BL_Flash_Init();

    if (!result)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "[1] Flash initialization FAILED"
        );

        return;
    }

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[1] Flash initialization PASSED"
    );


    /* ------------------------------------------------------------------------
     * Step 2: Erase
     * ---------------------------------------------------------------------- */

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[2] Erasing flash..."
    );

    result = BL_Flash_Erase(
        BL_FLASH_TEST_ADDRESS,
        BL_FLASH_TEST_LENGTH
    );

    if (!result)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "[2] Flash erase FAILED"
        );

        return;
    }

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[2] Flash erase PASSED"
    );


    /* ------------------------------------------------------------------------
     * Step 3: Check erased state
     * ---------------------------------------------------------------------- */

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[3] Checking erased state..."
    );

    result = BL_Flash_IsErased(
        BL_FLASH_TEST_ADDRESS,
        BL_FLASH_TEST_LENGTH
    );

    if (!result)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "[3] Erased check FAILED"
        );

        return;
    }

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[3] Erased check PASSED"
    );


    /* ------------------------------------------------------------------------
     * Step 4: Write
     * ---------------------------------------------------------------------- */

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[4] Writing test data..."
    );

    BL_Flash_Test_PrintData(
        flash_test_data,
        BL_FLASH_TEST_LENGTH
    );

    result = BL_Flash_Write(
        BL_FLASH_TEST_ADDRESS,
        flash_test_data,
        BL_FLASH_TEST_LENGTH
    );

    if (!result)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "[4] Flash write FAILED"
        );

        return;
    }

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[4] Flash write PASSED"
    );


    /* ------------------------------------------------------------------------
     * Step 5: Verify
     * ---------------------------------------------------------------------- */

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[5] Verifying flash..."
    );

    result = BL_Flash_Verify(
        BL_FLASH_TEST_ADDRESS,
        flash_test_data,
        BL_FLASH_TEST_LENGTH
    );

    if (!result)
    {
        LOG_ERROR(
            LOG_MODULE_BOOTLOADER,
            "[5] Flash verify FAILED"
        );

        return;
    }

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[5] Flash verify PASSED"
    );


    /* ------------------------------------------------------------------------
     * Step 6: Read back
     * ---------------------------------------------------------------------- */

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "[6] Read-back data:"
    );

    BL_Flash_Test_PrintData(
        (const uint8_t *)BL_FLASH_TEST_ADDRESS,
        BL_FLASH_TEST_LENGTH
    );


    /* ------------------------------------------------------------------------
     * Test Complete
     * ---------------------------------------------------------------------- */

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "========================================"
    );

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "FLASH TEST: ALL TESTS PASSED"
    );

    LOG_INFO(
        LOG_MODULE_BOOTLOADER,
        "========================================"
    );
}