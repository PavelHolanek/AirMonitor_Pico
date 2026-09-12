// SettingWidget.h - abstract base of a single row on the settings screen
#ifndef SETTING_WIDGET_H
#define SETTING_WIDGET_H

#include "core/NavigableWidget.h"

class SettingWidget : public NavigableWidget
{
public:
    SettingWidget(const wchar_t* title);
    virtual ~SettingWidget();

    // Points at a string literal in flash - the row never owns it or writes
    // through it. wchar_t because Text::Paint() maps codes above 127 through
    // getCharForWideChar(), which is what makes diacritics come out right.
    const wchar_t* title;

    // Paints the title left aligned and vertically centred in the row, in the
    // area's current colours - which is what makes the inverse look work.
    void drawTitle();

    // Left and right belong to the row itself; only enum rows do anything.
    virtual void moveLeft() {}
    virtual void moveRight() {}

    void buttonPressed() override {}

    // A selected row is drawn inverted: black text on white.
    void selected() override;
    void deselected() override;

    void update() override = 0;
    virtual void applySetting() = 0;
};

#endif // SETTING_WIDGET_H
