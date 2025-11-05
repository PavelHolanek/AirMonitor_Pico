#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <string.h>
#include <wchar.h>
#include "ili9341.h"
#include "gfx.h"
#include "Window.h"

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

#define TFT_WIDTH       320
#define TFT_HEIGHT      240
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
#define BACKGROUND 0x0000
#define FOREGROUND 0x73BF

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
    GFX_flush();
}

void InitializeDisplay(uint16_t color)
{
    // Initialize display
    puts("Initializing display...");
    LCD_setPins(TFT_DC, TFT_CS, TFT_RST, TFT_SCLK, TFT_MOSI);
    LCD_initDisplay();
    LCD_setRotation(TFT_ROTATION);
    GFX_createFramebuf();
    GFX_setClearColor(color);
    GFX_setTextBack(BACKGROUND);
    GFX_setTextColor(FOREGROUND);
    GFX_clearScreen();
}

int main()
{
    stdio_init_all();

    InitializeDisplay(FOREGROUND);

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

    MainWindow* window = new MainWindow{};

    return 0;
}
