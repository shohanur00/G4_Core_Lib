#include "version.h"

#include "version_config.h"

#include "logger.h"


/**
 * @brief Logs the current firmware version.
 *
 * Retrieves the firmware version from the build-generated
 * version configuration and outputs it through the Logger.
 *
 * The version is formatted as:
 *
 *     MAJOR.MINOR.BUILD
 *
 * Example:
 *
 *     Firmware Version: 1.0.42
 *
 * @pre LOG_Init() must be called before calling Version_LOG().
 *
 * @note The version components are generated during the
 *       build process and are not modified at runtime.
 */
void Version_LOG(void)
{
    LOG_INFO(LOG_MODULE_SYSTEM,
             "Firmware Version: %u.%u.%u",
             FW_VERSION_MAJOR,
             FW_VERSION_MINOR,
             FW_VERSION_BUILD);
}

