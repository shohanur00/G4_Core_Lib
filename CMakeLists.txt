# =============================================================================
# CMake Configuration
# =============================================================================

cmake_minimum_required(VERSION 3.20)

# -----------------------------------------------------------------------------
# Build / Indexing
# -----------------------------------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)


# -----------------------------------------------------------------------------
# Toolchain Configuration
# -----------------------------------------------------------------------------

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/target.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake)


# =============================================================================
# Project Definition
# =============================================================================

project(G4_Core_Lib)

enable_language(C ASM)

set(PROJECT_NAME G4_Core_Lib)

add_executable(${PROJECT_NAME})


# =============================================================================
# Build Target Configuration
#
# BUILD_TARGET:
#   FULL       -> Normal development build
#   BOOTLOADER -> Bootloader build
#   MAIN       -> Main firmware build
# =============================================================================

set(BUILD_TARGET "FULL" CACHE STRING "Firmware build target")

set_property(CACHE BUILD_TARGET PROPERTY STRINGS
    FULL
    BOOTLOADER
    MAIN
)


# -----------------------------------------------------------------------------
# Select Firmware Target
# -----------------------------------------------------------------------------

if(BUILD_TARGET STREQUAL "BOOTLOADER")

    set(BOOTLOADER_VALUE 1U)

    set(FLASH_ORIGIN 0x08000000)
    set(FLASH_LENGTH 16K)


elseif(BUILD_TARGET STREQUAL "MAIN")

    set(BOOTLOADER_VALUE 0U)

    set(FLASH_ORIGIN 0x08004000)
    set(FLASH_LENGTH 108K)


else()

    # FULL
    # Normal development firmware

    set(BOOTLOADER_VALUE 0U)

    set(FLASH_ORIGIN 0x08000000)
    set(FLASH_LENGTH 128K)

endif()


# =============================================================================
# Build Information
# =============================================================================

message(STATUS "========================================")
message(STATUS "Firmware Build Configuration")
message(STATUS "========================================")
message(STATUS "Build Type      : ${CMAKE_BUILD_TYPE}")
message(STATUS "Build Target    : ${BUILD_TARGET}")
message(STATUS "BOOTLOADER      : ${BOOTLOADER_VALUE}")
message(STATUS "FLASH ORIGIN    : ${FLASH_ORIGIN}")
message(STATUS "FLASH LENGTH    : ${FLASH_LENGTH}")
message(STATUS "========================================")


# =============================================================================
# Project Components
# =============================================================================

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/files.cmake)



# =============================================================================
# BIN and HEX Generator
# =============================================================================

add_custom_command(
    TARGET G4_Core_Lib
    POST_BUILD

    # Generate .bin
    COMMAND arm-none-eabi-objcopy
            -O binary
            $<TARGET_FILE:G4_Core_Lib>
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin

    # Generate .hex
    COMMAND arm-none-eabi-objcopy
            -O ihex
            $<TARGET_FILE:G4_Core_Lib>
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex

    COMMENT "Generating ${PROJECT_NAME}.bin and ${PROJECT_NAME}.hex"
)


# =============================================================================
# Source Files
# =============================================================================

target_sources(${PROJECT_NAME} PRIVATE

    # -------------------------------------------------------------------------
    # Application
    # -------------------------------------------------------------------------

    App/app.c


    # -------------------------------------------------------------------------
    # Drivers
    # -------------------------------------------------------------------------

    Drivers/gpio/gpio.c
    Drivers/gpio/gpio_hal.c

    Drivers/timecore/timecore.c
    Drivers/timecore/timecore_hal.c

    Drivers/systemclock/systemclock.c

    Drivers/uart/uart_driver.c


    # -------------------------------------------------------------------------
    # HAL
    # -------------------------------------------------------------------------

    HAL/uart/uart_hal.c


    # -------------------------------------------------------------------------
    # Services
    # -------------------------------------------------------------------------

    Services/debug/debug.c

    Services/logger/core/logger.c
    Services/logger/core/logger_hal.c

    Services/logger/backends/uart/logger_uart.c


    # -------------------------------------------------------------------------
    # Libraries
    # -------------------------------------------------------------------------

    Libraries/bit/bit.c

    Libraries/math/mbasic.c
    Libraries/math/map.c
    Libraries/math/interpolation.c
    Libraries/math/fixedpoint.c

    Libraries/containers/ringbuffer/ring_buffer.c

    Libraries/crc/crc.c

    # -------------------------------------------------------------------------
    # Firmware Version
    # -------------------------------------------------------------------------

    Version/version.c
    

)



if(BOOTLOADER_VALUE EQUAL 1)

    target_sources(${PROJECT_NAME} PRIVATE

    # -------------------------------------------------------------------------
    # Bootloader
    # -------------------------------------------------------------------------

    Bootloader/core/bootloader.c

    Bootloader/image/image.c

    Bootloader/protocol/bl_protocol.c

    Bootloader/transport/bl_transport.c
    Bootloader/transport/uart/bootloader_uart.c

    Bootloader/flash/bl_flash.c
    
    Bootloader/bootloader_app.c


    # -------------------------------------------------------------------------
    # Tests
    # -------------------------------------------------------------------------

    Bootloader/tests/bl_protocol_test.c
    Bootloader/tests/bl_flash_test.c


    # -------------------------------------------------------------------------
    # Other Source
    # -------------------------------------------------------------------------
    
    Bootloader/metadata/bl_metadata.c
    Bootloader/jump/bl_jump.c

    )

else()

    target_sources(${PROJECT_NAME} PRIVATE

    # -------------------------------------------------------------------------
    # Firmware
    # -------------------------------------------------------------------------

    Firmware/firmware.c

    )

endif()



# =============================================================================
# Include Directories
# =============================================================================

target_include_directories(${PROJECT_NAME} PRIVATE

    # -------------------------------------------------------------------------
    # Core / CMSIS
    # -------------------------------------------------------------------------

    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS


    # -------------------------------------------------------------------------
    # Application / Drivers
    # -------------------------------------------------------------------------

    ${CMAKE_CURRENT_SOURCE_DIR}/App
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP


    # -------------------------------------------------------------------------
    # Services
    # -------------------------------------------------------------------------

    ${CMAKE_CURRENT_SOURCE_DIR}/Services


    # -------------------------------------------------------------------------
    # Configuration
    # -------------------------------------------------------------------------

    ${CMAKE_CURRENT_SOURCE_DIR}/Config


    # -------------------------------------------------------------------------
    # Libraries
    # -------------------------------------------------------------------------

    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries


    # -------------------------------------------------------------------------
    # HAL
    # -------------------------------------------------------------------------

    ${CMAKE_CURRENT_SOURCE_DIR}/HAL


    # -------------------------------------------------------------------------
    # Bootloader
    # -------------------------------------------------------------------------

    ${CMAKE_CURRENT_SOURCE_DIR}/Bootloader


    # -------------------------------------------------------------------------
    # Firmware
    # -------------------------------------------------------------------------

    ${CMAKE_CURRENT_SOURCE_DIR}/Firmware

    # -------------------------------------------------------------------------
    # Version
    # -------------------------------------------------------------------------

    ${CMAKE_CURRENT_SOURCE_DIR}/Version

    ${CMAKE_BINARY_DIR}/generated
)


# =============================================================================
# Compiler Options
# =============================================================================

target_compile_options(${PROJECT_NAME} PRIVATE

    # User defined compiler options

)


# =============================================================================
# Compiler Definitions
# =============================================================================

target_compile_definitions(${PROJECT_NAME} PRIVATE

    BOOTLOADER=${BOOTLOADER_VALUE}

)


# =============================================================================
# Linker Script Configuration
#
# One linker template is used for all configurations.
# CMake generates the actual linker script inside the build directory.
# =============================================================================

set(LINKER_SCRIPT
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32g431xb_flash.ld
)

configure_file(
    ${LINKER_SCRIPT}
    ${CMAKE_BINARY_DIR}/stm32g431xb_flash.ld
    @ONLY
)


# =============================================================================
# Linker Options
# =============================================================================

target_link_options(${PROJECT_NAME} PRIVATE

    "-T${CMAKE_BINARY_DIR}/stm32g431xb_flash.ld"

)


# =============================================================================
# Linker Directories
# =============================================================================

target_link_directories(${PROJECT_NAME} PRIVATE

    # User defined linker search directories

)


# =============================================================================
# Linker Libraries
# =============================================================================

target_link_libraries(${PROJECT_NAME} PRIVATE

    # User defined linker libraries

)


# =============================================================================
# Firmware Versioning
#
# Version format:
#     Major.Minor.Build
#
# Example:
#     1.0.15
#
# Major / Minor are manually defined.
# Build number is automatically incremented on every build.
# =============================================================================

# -----------------------------------------------------------------------------
# Version Information
# -----------------------------------------------------------------------------

set(FW_VERSION_MAJOR 1)
set(FW_VERSION_MINOR 1)


# -----------------------------------------------------------------------------
# Version Paths
# -----------------------------------------------------------------------------

set(VERSION_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/Version
)

set(VERSION_STATE_FILE
    ${VERSION_DIR}/version_state.txt
)

set(VERSION_TEMPLATE
    ${VERSION_DIR}/version_config.h.in
)

set(VERSION_GENERATED_DIR
    ${CMAKE_BINARY_DIR}/generated
)

set(VERSION_GENERATED_HEADER
    ${VERSION_GENERATED_DIR}/version_config.h
)


# -----------------------------------------------------------------------------
# Generated Directory
# -----------------------------------------------------------------------------

file(MAKE_DIRECTORY
    ${VERSION_GENERATED_DIR}
)


# -----------------------------------------------------------------------------
# Version Generation
#
# This target runs on every build and increments the build number.
# -----------------------------------------------------------------------------

add_custom_target(GenerateVersion ALL

    COMMAND ${CMAKE_COMMAND}
        -DFW_VERSION_MAJOR=${FW_VERSION_MAJOR}
        -DFW_VERSION_MINOR=${FW_VERSION_MINOR}
        -DSTATE_FILE=${VERSION_STATE_FILE}
        -DTEMPLATE_FILE=${VERSION_TEMPLATE}
        -DOUTPUT_FILE=${VERSION_GENERATED_HEADER}
        -P ${VERSION_DIR}/generate_version.cmake

    BYPRODUCTS
        ${VERSION_GENERATED_HEADER}

    COMMENT
        "Generating firmware version"

    VERBATIM
)


# -----------------------------------------------------------------------------
# Version Include Directories
# -----------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PRIVATE

    ${VERSION_DIR}
    ${VERSION_GENERATED_DIR}

)


# -----------------------------------------------------------------------------
# Version Build Dependency
#
# GenerateVersion must run before the firmware is compiled.
# -----------------------------------------------------------------------------

add_dependencies(
    ${PROJECT_NAME}
    GenerateVersion
)