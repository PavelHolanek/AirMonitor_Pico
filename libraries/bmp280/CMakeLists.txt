add_library(bmp280
    bmp280.c
    bmp280.h
)

target_link_libraries(bmp280 hardware_i2c hardware_gpio pico_stdlib)

target_include_directories(bmp280 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")