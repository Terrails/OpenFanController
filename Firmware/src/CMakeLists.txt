cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(openfancontroller_firmware C CXX ASM)

set(PROJECT_DIR_PATH ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Project directory" FORCE)
message("PROJECT_DIR_PATH is ${PROJECT_DIR_PATH}")

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable.
add_executable(openfancontroller_firmware
        ${PROJECT_DIR_PATH}/Application/main.cpp
        ${PROJECT_DIR_PATH}/Application/emc230x.cpp
        ${PROJECT_DIR_PATH}/Application/fan_control.cpp
        ${PROJECT_DIR_PATH}/Application/host_communication.cpp
        ${PROJECT_DIR_PATH}/Application/logger.cpp
        ${PROJECT_DIR_PATH}/Application/usb_cdc.cpp
        ${PROJECT_DIR_PATH}/Application/usb_descriptors.c)

# Configure name and version
pico_set_program_name(openfancontroller_firmware "openfancontroller_firmware")
pico_set_program_version(openfancontroller_firmware "0.2")

# Add the standard library to the build
target_link_libraries(openfancontroller_firmware
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(openfancontroller_firmware PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${PROJECT_DIR_PATH}/Application
)

# Add any user requested libraries
target_link_libraries(openfancontroller_firmware 
        pico_multicore
        pico_stdlib
        hardware_flash
        hardware_i2c
        tinyusb_device
        pico_unique_id)

pico_add_extra_outputs(openfancontroller_firmware)

