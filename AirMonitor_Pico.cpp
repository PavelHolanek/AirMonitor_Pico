#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <string.h>
#include <wchar.h>
#include "ili9488.h"
#include "gfx.h"
#include "GUIManager.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Tasks.h"
#include "Clock.h"
#include "Log.h"
#include "Window.h"

#include "scd4x_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include <inttypes.h>  // PRIx64

#ifndef __DISPLAYTEST_H__
#define __DISPLAYTEST_H__

#define PICO_BREADBOARD_KIT 1

// Pin definitions for the LCD
#ifdef PICO_BREADBOARD_KIT
#define TFT_SCLK        6
#define TFT_MOSI        7
#define TFT_MISO        255 // Not required, used for DC...
#define TFT_DC          15
#define TFT_RST         14
#define TFT_CS          13
#define TFT_BACKLIGHT   255 // hardwired to 3.3v
#else
#define USE_SD_CARD     1
#define TFT_SCLK        18
#define TFT_MOSI        19
#define TFT_MISO        255 // Not required, used for DC...
#define TFT_DC          16
#define TFT_RST         21
#define TFT_CS          17
#define TFT_BACKLIGHT   255 // hardwired to 3.3v
#endif

#define TFT_WIDTH       480
#define TFT_HEIGHT      320
#define TFT_ROTATION    3

// Pin definitions for the SD card
#define SD_SCLK         10
#define SD_MOSI         11
#define SD_MISO         12
#define SD_CS           13

#define LINE_HEIGHT 24
#define DELAY 5

// Colors are in 565 (FFFF) format. To convert from RGB888 to RGB565, use:
//   ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

constexpr Color BACKGROUND = {0x00, 0x00, 0x00};
constexpr Color FOREGROUND  = {0xCC, 0xCC, 0xCC};

#endif

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

void printLine(uint16_t x, uint16_t y, const wchar_t *wideStr)
{
    size_t i = 0;
    size_t len = wcslen(wideStr);
    char charArray[len + 1];
    while (wideStr[i] != L'\0'  && i < len)
    {
        if (wideStr[i] > 127)
        {
            charArray[i] = getCharForWideChar(wideStr[i]);
        }
        else
        {
            charArray[i] = wideStr[i];
        }
        i++;
    }
    charArray[i] = '\0';
    GFX_setCursor(x, y);
    GFX_printf(3, charArray);
}

void convert_and_print_serial(uint16_t* serial_raw) { //scd41
    uint64_t serial_as_int = 0;
    sensirion_common_to_integer((uint8_t*)serial_raw, (uint8_t*)&serial_as_int,
                                LONG_INTEGER, 6);
    printf("0x%" PRIx64, serial_as_int);
}

void InitializeDisplay(Color color)
{
    // Initialize display
    puts("Initializing display...");
    LCD_setPins(TFT_DC, TFT_CS, TFT_RST, TFT_SCLK, TFT_MOSI);
    LCD_initDisplay();
    LCD_setRotation(TFT_ROTATION);

    GFX_setTextBack(BACKGROUND);
    GFX_setTextColor(FOREGROUND);
    GFX_clearScreen();
}

void initDiacritic()
{
    addExtraCharacter(L'Á');
    addExtraCharacter(L'Ó');
    addExtraCharacter(L'Í');
    addExtraCharacter(L'É');
    addExtraCharacter(L'Ú');
    addExtraCharacter(L'Ů');
    addExtraCharacter(L'Ě');
    addExtraCharacter(L'Ř');
    addExtraCharacter(L'Š');
    addExtraCharacter(L'Č');
    addExtraCharacter(L'Ž');
    addExtraCharacter(L'Ň');
    addExtraCharacter(L'Ď');
    addExtraCharacter(L'Ť');
    addExtraCharacter(L'Ý');

    addExtraCharacter(L'á');
    addExtraCharacter(L'ó');
    addExtraCharacter(L'í');
    addExtraCharacter(L'é');
    addExtraCharacter(L'ú');
    addExtraCharacter(L'ů');
    addExtraCharacter(L'ě');
    addExtraCharacter(L'ř');
    addExtraCharacter(L'š');
    addExtraCharacter(L'č');
    addExtraCharacter(L'ž');
    addExtraCharacter(L'ň');
    addExtraCharacter(L'ď');
    addExtraCharacter(L'ť');
    addExtraCharacter(L'ý');
}

static int counter = 0;
void vTaskIncrementPrint(void *pvParameters)
{
    while (1)
    {
        counter++;
        printf("Counter: %d\n", counter);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void scd41() {
    int16_t error = NO_ERROR;
    sensirion_i2c_hal_init();
    scd4x_init(SCD41_I2C_ADDR_62);

    uint16_t serial_number[3] = {0};
    sleep_ms(300);
    // Ensure sensor is in clean state
    error = scd4x_wake_up();
    if (error != NO_ERROR) {
        printf("error executing wake_up(): %i\n", error);
    }

    error = scd4x_stop_periodic_measurement();
    if (error != NO_ERROR) {
        printf("error executing stop_periodic_measurement(): %i\n", error);
    }

    error = scd4x_reinit();
    if (error != NO_ERROR) {
        printf("error executing reinit(): %i\n", error);
    }

    // Read out information about the sensor
    error = scd4x_get_serial_number(serial_number, 3);
    if (error != NO_ERROR) {
        printf("error executing get_serial_number(): %i\n", error);
        return;
    }
    printf("serial number: ");
    convert_and_print_serial(serial_number);
    printf("\n");
    //
    // If temperature offset and/or sensor altitude compensation
    // is required, you should call the respective functions here.
    // Check out the header file for the function definitions.
    // Start periodic measurements (5sec interval)
    error = scd4x_start_periodic_measurement();
    if (error != NO_ERROR) {
        printf("error executing start_periodic_measurement(): %i\n", error);
        return;
    }
    //
    // If low-power mode is required, switch to the low power
    // measurement function instead of the standard measurement
    // function above. Check out the header file for the definition.
    //
    bool data_ready = false;
    uint16_t co2_concentration = 0;
    int32_t temperature = 0;
    int32_t relative_humidity = 0;
    uint16_t repetition = 0;
    for (repetition = 0; repetition < 50; repetition++) {
        //
        // Slow down the sampling to 0.2Hz.
        //
        sleep_ms(5000);
        //
        // If ambient pressure compensation during measurement
        // is required, you should call the respective functions here.
        // Check out the header file for the function definition.
        error = scd4x_get_data_ready_status(&data_ready);
        if (error != NO_ERROR) {
            printf("error executing get_data_ready_status(): %i\n", error);
            continue;
        }
        while (!data_ready) {
            sleep_ms(1000);
            error = scd4x_get_data_ready_status(&data_ready);
            if (error != NO_ERROR) {
                printf("error executing get_data_ready_status(): %i\n", error);
                continue;
            }
        }
        error = scd4x_read_measurement(&co2_concentration, &temperature,
                                       &relative_humidity);
        if (error != NO_ERROR) {
            printf("error executing read_measurement(): %i\n", error);
            continue;
        }
        //
        // Print results in physical units.
        printf("CO2 concentration [ppm]: %u\n", co2_concentration);
        printf("Temperature [m°C] : %i\n", temperature);
        printf("Humidity [mRH]: %i\n", relative_humidity);
    }

    return;
}

int main()
{
    stdio_init_all();
/*
    scd41();

    initClock();

    sleep_ms(10000);
    LOG("Initializing");
    sleep_ms(2000);
    LOG("Initializing");

    */

    sleep_ms(100);

    InitializeDisplay(FOREGROUND);

    initDiacritic();

    gui_init();

    sleep_ms(100);

    intializeSemaphoresAndQueues();

    //xTaskCreate(getClockTimeTask,           "getClockTimeTask",           1000, NULL, 1, NULL);
    xTaskCreate(readBME280Task,             "readBME280Task",             1000, NULL, 1, NULL);
    xTaskCreate(readSHT40Task,              "readSHT40Task",              1000, NULL, 1, NULL);
    xTaskCreate(readSCD41Task,              "readSCD41Task",              1000, NULL, 1, NULL);
    xTaskCreate(dataManagerTask,            "dataManagerTask",            1000, NULL, 1, NULL);
    xTaskCreate(valuesChangedGUITask,       "valuesChangedGUITask",       1000, NULL, 1, NULL);
    //xTaskCreate(timeChangedGUITask,         "timeChangedGUITask",         1000, NULL, 1, NULL);
    //xTaskCreate(joystickEvaluationTask,     "joystickEvaluationTask",     1000, NULL, 1, NULL);
    //xTaskCreate(joystickActionGUITask,      "joystickActionGUITask",      1000, NULL, 1, NULL);
    //xTaskCreate(writeLogTask,               "writeLogTask",               1000, NULL, 1, NULL);
    //xTaskCreate(writeValueToStorageTask,    "writeValueToStorageTask",    1000, NULL, 1, NULL);
    vTaskStartScheduler();

    for (;;) {
    }
    return 0;
}
