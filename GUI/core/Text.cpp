#include "GraphicElement.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include <wchar.h>

Text::Text(const wchar_t* s) : GraphicElement(), str(s), textSize(1) {}
Text::Text(const wchar_t* s, uint16_t x, uint16_t y) : GraphicElement(x, y), str(s), textSize(1) {}
Text::~Text() {}

void Text::Paint()
{
    size_t i = 0;
    size_t len = wcslen(str);
    char charArray[len + 1];
    while (str[i] != L'\0' && i < len)
    {
        if (str[i] > 127)
        {
            charArray[i] = getCharForWideChar(str[i]);
        }
        else
        {
            charArray[i] = (char)str[i];
        }
        i++;
    }

    charArray[i] = '\0';
    GFX_setCursor(posX, posY);
    GFX_setTextColor(color);
    GFX_setTextBack(backgroundColor);
    GFX_printf(textSize, charArray);
}

