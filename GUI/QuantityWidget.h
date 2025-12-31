#ifndef QUANTITY_WIDGET_H
#define QUANTITY_WIDGET_H

#include <cstdint>
#include <wchar.h>
#include "NavigableWidget.h"
#include "GraphicElement.h"
#include "Base.h"

class QuantityWidget : public NavigableWidget
{
public:
    QuantityWidget(QUANTITY t);
    virtual ~QuantityWidget();

    void setValue(int32_t value);
    void update() override;
    void initialize();

    uint8_t (*iconBitmap)[288];

    const char *units;
    QUANTITY type;

    void selected() override;
    void deselected() override;
private:
    bitMap48* pictogram;
    bitMap32* arrow;
    Text* valueText;
    Text* unitsText;
    bool childrenAdded;
    bool initialized;
    wchar_t valueBuffer[16];
    wchar_t unitsBuffer[12];
};

#endif // QUANTITY_WIDGET_H

