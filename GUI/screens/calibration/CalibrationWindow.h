// CalibrationWindow.h - manual per-quantity offsets added to a reading
#ifndef CALIBRATION_WINDOW_H
#define CALIBRATION_WINDOW_H

#include <stdint.h>
#include "core/Window.h"
#include "core/GraphicElement.h"
#include "core/HoldRepeat.h"

// Temperature, humidity, CO2 and pressure - one row each, all on screen at
// once, so this screen needs no scrolling.
#define CALIBRATION_ROWS_COUNT 4

// Holding left or right keeps stepping: the first repeat after the delay, then
// one every period.
#define CALIBRATION_REPEAT_DELAY_MS 700U
#define CALIBRATION_REPEAT_PERIOD_MS 500U

class CalibrationWindow : public Window
{
public:
    CalibrationWindow();
    virtual ~CalibrationWindow();

    void joystickEvent(const JoystickEvent& event) override;
    void timerExpired(GuiTimer* timer) override;
    void enterWindow() override;

private:
    // Steps the selected offset by one flick. Clamped, not wrapped.
    void changeCurrent(int8_t direction);
    void stepValue(JOYSTICK_DIRECTION direction);

    void moveSelection(uint8_t newRow);
    void paintRow(uint8_t index);

    // Rows are fixed and uniform, so the screen owns plain areas and paints
    // them itself instead of carrying a widget object per row.
    Area* rowAreas[CALIBRATION_ROWS_COUNT];
    uint8_t currentRow;

    HoldRepeat valueRepeat;
};

#endif // CALIBRATION_WINDOW_H
