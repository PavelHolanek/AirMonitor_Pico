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
        GUI/TimePickerWiget.cpp
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
        GUI/TimePickerWiget.h
        GUI/GUIManager.h
        GUI/GUIManager.cpp
        GUI/Bitmaps.h
        Base.h
        Clock.c
        Clock.h
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
add_subdirectory(libraries/embedded-i2c-scd4x)
add_subdirectory(libraries/embedded-i2c-sht4x)
add_subdirectory(libraries/bmp280)

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
		scd4x
        sht4x
		bmp280
        )

pico_add_extra_outputs(AirMonitor_Pico)
