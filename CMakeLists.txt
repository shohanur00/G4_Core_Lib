# Set the minimum version of CMake that can be used
cmake_minimum_required(VERSION 3.20)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Set target definition variables required by the toolchain file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/target.cmake)

# Set the toolchain file for cross-compilation
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake)

# Project definition
project(G4_Core_Lib)

# Enable CMake support for ASM and C/CPP languages
enable_language(C ASM)

set(PROJECT_NAME G4_Core_Lib)

# Create an executable object type
add_executable(${PROJECT_NAME})


# Include additional CMake files for flags, components, and files
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/files.cmake)

# Add sources to executable/library
target_sources(${PROJECT_NAME} PRIVATE
  # User defined sources
  App/app.c
  BSP/board.c
  Drivers/gpio/gpio.c
  Drivers/timecore/timecore.c
  Drivers/timecore/timecore_hal.c
  Services/Debug/debug.c
  Drivers/gpio/gpio_hal.c
  Drivers/systemclock/systemclock.c
  Services/Logger/Core/logger.c
  Services/Logger/Core/logger_hal.c
  Services/Logger/Backends/UART/logger_uart.c
)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
  # User defined include directories
  ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS
  ${CMAKE_CURRENT_SOURCE_DIR}/Version
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/systemclock
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/gpio
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/timecore
  ${CMAKE_CURRENT_SOURCE_DIR}/App
  ${CMAKE_CURRENT_SOURCE_DIR}/BSP
  ${CMAKE_CURRENT_SOURCE_DIR}/Services/Debug
  ${CMAKE_CURRENT_SOURCE_DIR}/Services/Logger/Frontend
  ${CMAKE_CURRENT_SOURCE_DIR}/Config
  ${CMAKE_BINARY_DIR}/generated
)

# Compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
  # User defined compiler options
)

# Add additional defined symbols
target_compile_definitions(${PROJECT_NAME} PRIVATE
  # Add additional defined symbols here
)

# Linker options
target_link_options(${PROJECT_NAME} PRIVATE 
  # User defined linker options
)

# Add additional linker search directories
target_link_directories(${PROJECT_NAME} PRIVATE
  # Add additional linker search directories here
)

# Add additional linker libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
  # Add additional linker libraries here
)







# ===========================================================================
#   FIRMWARE VERSIONING
#   Auto-incrementing A.B.C version scheme (Major.Minor manual, Build auto)
# ===========================================================================

# ---------------------------------------------------------------------------
# 1. Version variables and paths
# ---------------------------------------------------------------------------
set(FW_VERSION_MAJOR 1)
set(FW_VERSION_MINOR 0)

set(VERSION_DIR              ${CMAKE_SOURCE_DIR}/Version)
set(VERSION_STATE_FILE       ${VERSION_DIR}/version_state.txt)
set(VERSION_TEMPLATE         ${VERSION_DIR}/version_config.h.in)
set(VERSION_GENERATED_DIR    ${CMAKE_BINARY_DIR}/generated)
set(VERSION_GENERATED_HEADER ${VERSION_GENERATED_DIR}/version_config.h)

file(MAKE_DIRECTORY ${VERSION_GENERATED_DIR})

# ---------------------------------------------------------------------------
# 2. Custom target that regenerates the version header on every build
#    (no tracked OUTPUT -> always re-runs, not just on CMake reconfigure)
# ---------------------------------------------------------------------------
add_custom_target(GenerateVersion ALL
    COMMAND ${CMAKE_COMMAND}
        -DFW_VERSION_MAJOR=${FW_VERSION_MAJOR}
        -DFW_VERSION_MINOR=${FW_VERSION_MINOR}
        -DSTATE_FILE=${VERSION_STATE_FILE}
        -DTEMPLATE_FILE=${VERSION_TEMPLATE}
        -DOUTPUT_FILE=${VERSION_GENERATED_HEADER}
        -P ${VERSION_DIR}/generate_version.cmake
    BYPRODUCTS ${VERSION_GENERATED_HEADER}
    COMMENT "Bumping firmware build number"
    VERBATIM
)

# ---------------------------------------------------------------------------
# 3. Main executable target
#    (already declared earlier in this file — must exist before this point)
# ---------------------------------------------------------------------------
# add_executable(${PROJECT_NAME})

# ---------------------------------------------------------------------------
# 4. Add version.c to the existing source list
# ---------------------------------------------------------------------------
target_sources(${PROJECT_NAME} PRIVATE
    Version/version.c
    # ... rest of your existing sources (Src/, App/, Drivers/, BSP/, Services/, etc.)
)

# ---------------------------------------------------------------------------
# 5. Version-related include paths and build dependency
# ---------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${VERSION_DIR}
    ${VERSION_GENERATED_DIR}
)
add_dependencies(${PROJECT_NAME} GenerateVersion)