#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
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
#include "sensor_bmp280.h"
#include "Parameters.h"

#define MOSFET_5V_PIN 17
#define MOSFET_3V3_PIN 16

#define TFT_SCLK        6
#define TFT_MOSI        7
#define TFT_MISO        255
#define TFT_DC          15
#define TFT_RST         14
#define TFT_CS          13
#define TFT_BACKLIGHT   255
#define TFT_WIDTH       480
#define TFT_HEIGHT      320
#define TFT_ROTATION    3

constexpr Color BACKGROUND = {0x00, 0x00, 0x00};
constexpr Color FOREGROUND  = {0xCC, 0xCC, 0xCC};



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

void InitializeDisplay()
{
    puts("Initializing display...");
    LCD_setPins(TFT_DC, TFT_CS, TFT_RST, TFT_SCLK, TFT_MOSI);
    LCD_initDisplay();
    LCD_setRotation(TFT_ROTATION);
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

#define GPIO_PUSH_PIN 3
#define GPIO_MOVE_PIN 9

void joystickCallback(uint gpio, uint32_t events)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (GPIO_PUSH_PIN == gpio)
    {
        xSemaphoreGiveFromISR(JoystickPressInteruptionSemaphore, &xHigherPriorityTaskWoken);
    }
    else if (GPIO_MOVE_PIN == gpio)
    {
        xSemaphoreGiveFromISR(JoystickMoveInteruptionSemaphore, &xHigherPriorityTaskWoken);
    }
}

int16_t initializationScreenTextPosition = 10;
void printOnTheScreem(const char* text, uint8_t size = 2)
{
    GFX_setCursor(20, initializationScreenTextPosition);
    GFX_setTextColor(PARAM_COLOR_BLACK);
    GFX_setTextBack(PARAM_COLOR_WHITE);
    GFX_printf(size, text);
    initializationScreenTextPosition += 25;
}

int main()
{
    stdio_init_all();

    sleep_ms(200);

    gpio_init(MOSFET_5V_PIN);
    gpio_set_dir(MOSFET_5V_PIN, GPIO_OUT);
    gpio_init(MOSFET_3V3_PIN);
    gpio_set_dir(MOSFET_3V3_PIN, GPIO_OUT);
    gpio_put(MOSFET_3V3_PIN, 0);
    gpio_put(MOSFET_5V_PIN, 0);
    sleep_ms(1000);
    gpio_put(MOSFET_5V_PIN, 1);
    gpio_put(MOSFET_3V3_PIN, 1);

    printf("Initializing display");
    InitializeDisplay();
    initDiacritic();

    printOnTheScreem("Version:");
    {
        char commit[41];
        snprintf(commit, 41, "%s", COMMIT_SHA);
        printOnTheScreem(commit, 1);
    }

    printOnTheScreem("Initializing clock");
    initClock();
    sleep_ms(250);

    printOnTheScreem("Initializing bmp280");
    sensor_bmp280_init();

    printOnTheScreem("Initializing sht40");
    sensor_sht40_init();
    
    printOnTheScreem("Initializing sdc41");
    sensor_sdc41_init();

    printOnTheScreem("Initializing joystick");
    gpio_init(GPIO_PUSH_PIN);
    gpio_set_irq_enabled_with_callback(GPIO_PUSH_PIN, GPIO_IRQ_EDGE_FALL, true, &joystickCallback);
    gpio_init(GPIO_MOVE_PIN);
    gpio_set_irq_enabled_with_callback(GPIO_MOVE_PIN, GPIO_IRQ_EDGE_FALL, true, &joystickCallback);

    sleep_ms(100);

    printOnTheScreem("Initializing freeRTOS kernel");
    intializeSemaphoresAndQueues();
    xTaskCreate(getClockTimeTask,           "getClockTimeTask",           1000, NULL, 8, NULL);
    xTaskCreate(setClockTimeTask,           "setClockTimeTask",           1000, NULL, 8, NULL);
    xTaskCreate(readbmp280Task,             "readbmp280Task",             1000, NULL, 2, NULL);
    xTaskCreate(readSHT40Task,              "readSHT40Task",              1000, NULL, 2, NULL);
    xTaskCreate(readSCD41Task,              "readSCD41Task",              1000, NULL, 2, NULL);
    xTaskCreate(dataManagerTask,            "dataManagerTask",            1000, NULL, 1, NULL);
    xTaskCreate(timeChangedGUITask,         "timeChangedGUITask",         1000, NULL, 1, NULL);
    xTaskCreate(joystickPressedTask,        "joystickPressedTask",        1000, NULL, 10, NULL);
    xTaskCreate(joystickMovedTask,          "joystickMovedTask",          1000, NULL, 10, NULL);
    xTaskCreate(joystickEvaluationTask,     "joystickEvaluationTask",     1000, NULL, 9, NULL);
    //xTaskCreate(writeLogTask,               "writeLogTask",               1000, NULL, 1, NULL);
    //xTaskCreate(writeValueToStorageTask,    "writeValueToStorageTask",    1000, NULL, 1, NULL);
    
    sleep_ms(1000);

    gui_init();
    
    vTaskStartScheduler();

    for (;;)
    {}
    return 0;
}
