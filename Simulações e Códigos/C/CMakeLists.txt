# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add your source files
set(SOURCES
    medicamento_monitor.c)
    add_executable(PicoMedAlert
    medicamento_monitor.c)
    # Add other source files here

# Include directories
#include_directories
#   ${CMAKE_SOURCE_DIR}/include
#    # Add other include directories here
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pico)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pico/cyw43_arch)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hardware)

# Inclui diretórios necessários
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/pico
    ${CMAKE_CURRENT_SOURCE_DIR}/pico/cyw43_arch
    ${CMAKE_CURRENT_SOURCE_DIR}/hardware
)


# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
project(PicoMedAlert C CXX ASM)

# Define o caminho para o SDK do Pico
set(PICO_SDK_PATH "C:/Users/User/.pico-sdk/sdk/2.1.1/src/common/pico_base_headers/include/pico/veersion.h.in" CACHE PATH "Path to the Pico SDK")

# Inclui o SDK
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(PicoMedAlert medicamento_monitor.c )

pico_set_program_name(PicoMedAlert "PicoMedAlert")
pico_set_program_version(PicoMedAlert "0.1")

# Generate PIO header
pico_generate_pio_header(PicoMedAlert ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(PicoMedAlert 0) # Set this to 1 to enable UART output
pico_enable_stdio_usb(PicoMedAlert 1) # Set this to 1 to enable USB output

# Add the standard library to the build
target_link_libraries(PicoMedAlert
        pico_stdlib # Add this line to include the standard library
        hardware_dma  # Add this line to include the DMA driver
        hardware_uart # Add this line to include the UART driver
        hardware_gpio # Add this line to include the GPIO driver
        hardware_cyw43_arch_lwip_threadsafe_background # Add this line to include the CYW4343W WiFi driver
        hardware_stdlib # Add this line to include the standard library driver
        hardware_pwm # Add this line to include the PWM driver
)
# Add the standard include files to the build
target_include_directories(PicoMedAlert PRIVATE /pico/pico-sdk/src/rp2_common/
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/pico/pico-sdk/src/rp2_common/hardware.dma,
        ${CMAKE_CURRENT_LIST_DIR}/pico/pico-sdk/src/rp2_common/hardware_gpio,
        ${CMAKE_CURRENT_LIST_DIR}/pico/pico-sdk/src/rp2_common/hardware_uart,

)
# Create map/bin/hex/uf2 files
pico_add_extra_outputs(PicoMedAlert)

