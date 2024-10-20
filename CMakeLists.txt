# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()


set(BOARD_NAME W55RP20_EVB_PICO)
set(WIZNET_CHIP W5500)
add_definitions(-D_WIZCHIP_=W5500)
add_definitions(-DDEVICE_BOARD_NAME=W55RP20_EVB_PICO)
message(STATUS "WIZNET_CHIP = ${WIZNET_CHIP}")

# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(W55RP20_SSM_ETHER_HDMI C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()



include_directories(
    ioLibrary_Driver/Internet/httpServer
	ioLibrary_Driver/Ethernet
   	ioLibrary_Driver/Ethernet/w5500
	)

#add_subdirectory(ioLibrary_Driver/Ethernet/w5500)

# Add executable. Default name is the project name, version 0.1

add_executable(W55RP20_SSM_ETHER_HDMI W55RP20_SSM_ETHER_HDMI.cpp 
ioLibrary_Driver/Ethernet/wizchip_conf.c
ioLibrary_Driver/Ethernet/socket.c

ioLibrary_Driver/Ethernet/w5500/w5x00_gpio_irq.c
ioLibrary_Driver/Ethernet/w5500/wiznet_spi_pio.c
ioLibrary_Driver/Internet/httpServer/httpParser.c
ioLibrary_Driver/Internet/httpServer/httpServer.c
ioLibrary_Driver/Internet/httpServer/httpUtil.c
ioLibrary_Driver/Ethernet/w5500/w5500.c
ioLibrary_Driver/Ethernet/w5500/w5x00_spi.c
)

pico_set_program_name(W55RP20_SSM_ETHER_HDMI "W55RP20_SSM_ETHER_HDMI")
pico_set_program_version(W55RP20_SSM_ETHER_HDMI "0.1")

# Generate PIO header
#pico_generate_pio_header(W55RP20_Ethernet ${CMAKE_CURRENT_LIST_DIR}/ioLibrary_Driver/Ethernet/w5500/wiznet_spi_pio.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(W55RP20_SSM_ETHER_HDMI 0)
pico_enable_stdio_usb(W55RP20_SSM_ETHER_HDMI 1)

# Add the standard library to the build
target_link_libraries(W55RP20_SSM_ETHER_HDMI
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(W55RP20_SSM_ETHER_HDMI PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
  ${CMAKE_CURRENT_LIST_DIR}/ioLibrary_Driver
  ${CMAKE_CURRENT_LIST_DIR}/ioLibrary_Driver/Ethernet/w5500
  ${CMAKE_CURRENT_LIST_DIR}/ioLibrary_Driver/Internet/httpServer
)




# Add any user requested libraries
target_link_libraries(W55RP20_SSM_ETHER_HDMI 
        hardware_spi
        hardware_dma
        hardware_pio
        hardware_timer

        pico_multicore
        pico_util
        libdvi
        libsprite
        )


pico_add_extra_outputs(W55RP20_SSM_ETHER_HDMI)


include_directories(
	PicoDvi/include
    )

add_subdirectory(PicoDvi)    

