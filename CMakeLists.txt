# Generated Cmake Pico project file

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
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

include(FreeRTOS_Kernel_import.cmake)

project(AirMonitor_Pico C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

# Run the git command to get the commit SHA
execute_process(
    COMMAND git rev-parse --verify HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}  # Ensure the command runs from the root of your repo
    OUTPUT_VARIABLE COMMIT_SHA
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Define the commit SHA as a preprocessor macro
add_definitions(-DCOMMIT_SHA="${COMMIT_SHA}")

add_executable(AirMonitor_Pico AirMonitor_Pico.cpp
        GUI/GraphicElement.cpp
        GUI/Area.cpp
        GUI/Text.cpp
        GUI/Line.cpp
        GUI/BitMap32.cpp
        GUI/BitMap48.cpp
        GUI/NavigableWidget.cpp
        GUI/QuantityWidget.cpp
        GUI/SettingsWidget.cpp
        GUI/TimeWidget.cpp
        GUI/Window.cpp
        GUI/MainWindow.cpp
        GUI/SettingWindow.cpp
        GUI/ClockWindow.cpp
        GUI/GraphWindow.cpp
        GUI/TimePickerWiget.cpp
        GUI/GraphWidget.cpp
        GUI/GraphicElement.h
        GUI/Widget.h
        GUI/Widget.cpp
        GUI/NavigableWidget.h
        GUI/QuantityWidget.h
        GUI/SettingsWidget.h
        GUI/TimeWidget.h
        GUI/Window.h
        GUI/MainWindow.h
        GUI/SettingWindow.h
        GUI/ClockWindow.h
        GUI/GraphWindow.h
        GUI/TimePickerWiget.h
        GUI/GraphWidget.h
        GUI/GraphData.h
        GUI/GraphData.c
        GUI/GraphAlgorithm_LinearInterpolation.c
        GUI/GraphAlgorithm_BucketAverage.c
        GUI/GUIManager.h
        GUI/GUIManager.cpp
        GUI/Bitmaps.h
        Base.h
        Pinout.h
        Clock.c
        Clock.h
        dataManager.c
        dataManager.h
        Settings.h
        Settings.c
        Tasks.c
        Tasks.h
        Log.c
        Log.h
        sensors/sensor_sdc41.c
        sensors/sensor_sdc41.h
        sensors/sensor_sht40.c
        sensors/sensor_sht40.h
        sensors/sensor_bmp280.c
        sensors/sensor_bmp280.h
        sensors/sensors_i2c.c
        sensors/sensors_i2c.h
)

pico_set_program_name(AirMonitor_Pico "AirMonitor_Pico")
pico_set_program_version(AirMonitor_Pico "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(AirMonitor_Pico 0)
pico_enable_stdio_usb(AirMonitor_Pico 1)

add_subdirectory(libraries/pico-displayDrivs/ili9488)
add_subdirectory(libraries/pico-displayDrivs/gfx)
add_subdirectory(libraries/pico-extras)
add_subdirectory(libraries/ds3231)
add_subdirectory(libraries/bmp280)

#Sensirion sensors SHT40 and SCD41
set(SENSIRION_SHT4X ${CMAKE_CURRENT_LIST_DIR}/libraries/embedded-i2c-sht4x)
set(SENSIRION_SCD4X ${CMAKE_CURRENT_LIST_DIR}/libraries/embedded-i2c-scd4x)

foreach(shared IN ITEMS sensirion_common.c sensirion_common.h
                        sensirion_i2c.c sensirion_i2c.h
                        sensirion_i2c_hal.h sensirion_config.h)
    file(SHA256 ${SENSIRION_SHT4X}/${shared} sht4x_hash)
    file(SHA256 ${SENSIRION_SCD4X}/${shared} scd4x_hash)
    if(NOT sht4x_hash STREQUAL scd4x_hash)
        message(FATAL_ERROR
            "${shared} differs between the SHT4x and SCD4x submodules. "
            "The shared Sensirion code can no longer be compiled just once - "
            "split the sensirion target into two libraries first.")
    endif()
endforeach()

add_library(sensirion
        ${SENSIRION_SHT4X}/sht4x_i2c.c
        ${SENSIRION_SCD4X}/scd4x_i2c.c
        ${SENSIRION_SHT4X}/sensirion_common.c
        ${SENSIRION_SHT4X}/sensirion_i2c.c
        sensors/sensirion/sensirion_i2c_hal.c
)

target_include_directories(sensirion PUBLIC
        ${SENSIRION_SHT4X}
        ${SENSIRION_SCD4X}
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/sensors
)

target_link_libraries(sensirion hardware_i2c hardware_gpio pico_stdlib)

# Add the standard library to the build
target_link_libraries(AirMonitor_Pico
		pico_async_context_freertos
        FreeRTOS-Kernel-Heap4
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(AirMonitor_Pico PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${CMAKE_CURRENT_LIST_DIR}/GUI
        ${CMAKE_CURRENT_LIST_DIR}/sensors
)

# Add any user requested libraries
target_link_libraries(AirMonitor_Pico 
        hardware_spi
		hardware_adc
        ili9488
        gfx
		pico_ds3231
		sensirion
		bmp280
        )

pico_add_extra_outputs(AirMonitor_Pico)
