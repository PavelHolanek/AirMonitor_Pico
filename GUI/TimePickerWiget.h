// TimePickerWiget.h - simple time selection widget placeholder (HH::MM)
#ifndef TIME_PICKER_WIGET_H
#define TIME_PICKER_WIGET_H

#include "NavigableWidget.h"
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
    Text* text;
    wchar_t buffer[16];
    Text* hint;
    int hintPos; // 0..1 position corresponding to Hours(0) or Minutes(1)
    wchar_t hintBuffer[8];
    Time selectedTime;
};

#endif // TIME_PICKER_WIGET_H
