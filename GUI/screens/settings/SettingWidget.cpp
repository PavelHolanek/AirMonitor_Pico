// SettingWidget.cpp - shared painting of a settings row
#include "SettingWidget.h"
#include "Parameters.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"

SettingWidget::SettingWidget(const wchar_t* title)
    : NavigableWidget(), title(title)
{
}

SettingWidget::~SettingWidget()
{
}

void SettingWidget::drawTitle()
{
    if (!area || !title)
    {
        return;
    }

    // Transient - Text only reads its members while painting, so there is no
    // reason to keep one allocated per row.
    Text text(title,
              area->posX + SETTINGS_ROW_PADDING,
              area->posY + (SETTINGS_ROW_HEIGHT - SETTINGS_ROW_TEXT_HEIGHT) / 2);
    text.textSize = SETTINGS_TITLE_TEXT_SIZE;
    text.color = area->color;
    text.backgroundColor = area->backgroundColor;
    text.Paint();
}

void SettingWidget::paintBuffered()
{
    if (!area)
    {
        return;
    }

    const bool ownsFramebuf = GFX_createFramebuf(area->posX, area->posY,
                                                 area->sizeX, area->sizeY);
    if (ownsFramebuf)
    {
        // The row is painted as a rounded rectangle, so the four corner cuts are
        // never written and would be flushed as whatever the buffer held before.
        // Not area->backgroundColor - that is white on a selected row, which would
        // square the corners off; the screen behind the row is black.
        GFX_clearFramebuf(PARAM_COLOR_BLACK);
    }

    update();

    // Without ownership somebody above us opened the buffer and will flush it;
    // tearing it down here would drop their drawing.
    if (ownsFramebuf)
    {
        GFX_flush();
        GFX_destroyFramebuf();
    }
}

void SettingWidget::selected()
{
    if (!area)
    {
        return;
    }
    area->backgroundColor = PARAM_COLOR_WHITE;
    area->color = PARAM_COLOR_BLACK;
    paintBuffered();
}

void SettingWidget::deselected()
{
    if (!area)
    {
        return;
    }
    area->backgroundColor = PARAM_COLOR_BLACK;
    area->color = PARAM_COLOR_WHITE;
    paintBuffered();
}
