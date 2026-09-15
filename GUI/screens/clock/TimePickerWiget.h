// TimePickerWiget.h - date and time selection widget (DD.MM. HH:MM)
#ifndef TIME_PICKER_WIGET_H
#define TIME_PICKER_WIGET_H

#include "core/NavigableWidget.h"
#include "Clock.h"
class TimePickerWiget : public NavigableWidget
{
public:
    TimePickerWiget();
    virtual ~TimePickerWiget();

    void update() override;
    void selected() override;
    void deselected() override;

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void onPressed();
    void setSelectedTime(const Time& t);
    Time getSelectedTime() const;

private:
    // Editable fields, in the order they appear in the rendered string
    enum Field
    {
        FIELD_DAY = 0,
        FIELD_MONTH,
        FIELD_HOUR,
        FIELD_MINUTE,
        FIELD_COUNT
    };

    void clampDayToMonth();

    Text* text;
    wchar_t buffer[24];
    Text* hint;
    int hintPos; // one of Field - which value the up/down keys change
    wchar_t hintBuffer[24];
    Time selectedTime;
};

#endif // TIME_PICKER_WIGET_H
