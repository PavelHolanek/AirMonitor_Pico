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
#include <inttypes.h>  // PRIx64
#include "sensor_sdc41.h"
#include "sensor_sht40.h"
#include "sensor_bme280.h"

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

int main()
{
    stdio_init_all();

    sleep_ms(10000);

    printf("Initializing clock\n");
    //initClock();
    sleep_ms(250);

    printf("Initializing bmp280\n");
    sensor_bme280_init();

    printf("Initializing sht40\n");
    sensor_sht40_init();
    
    printf("Initializing sdc41\n");
    sensor_sdc41_init();

    printf("Initializing display\n");
    InitializeDisplay(FOREGROUND);
    initDiacritic();
    gui_init();

    sleep_ms(100);

    printf("Initializing freeRTOS kernel\n");
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
