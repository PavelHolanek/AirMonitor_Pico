#ifndef GRAPHIC_ELEMENT_H
#define GRAPHIC_ELEMENT_H

#include "Base.h"
#include "Libraries/pico-displayDrivs/ili9488/ili9488.h" // brings Color struct definition

#define MAX_AREA_CHILDREN_COUNT 4

class GraphicElement
{
public:
    Color backgroundColor;
    Color color;
    uint16_t posX;
    uint16_t posY;

    GraphicElement();
    GraphicElement(uint16_t x, uint16_t y);
    virtual ~GraphicElement();
    virtual void Paint() = 0;
};

class Area : public GraphicElement
{
public:
    Area();
    Area(uint16_t x, uint16_t y, uint16_t sx, uint16_t sy);
    virtual ~Area();
    void Paint() override;
    uint16_t sizeX;
    uint16_t sizeY;

    inline uint16_t getEndX() const {return posX + sizeX ;};
    inline uint16_t getEndY() const {return posY + sizeY ;};

    void addChildren(GraphicElement* child);

private:
    GraphicElement* children[MAX_AREA_CHILDREN_COUNT];
    uint8_t numberOfChildren;
};

class Text : public GraphicElement
{
public:
    Text(const wchar_t*);
    Text(const wchar_t* s, uint16_t x, uint16_t y);
    virtual ~Text();
    void Paint() override;
    const wchar_t* str;
    uint8_t textSize;
};

class Line : public GraphicElement
{
public:
    Line();
    virtual ~Line();
    void Paint() override;
    uint16_t endPosX;
    uint16_t endPosY;
};

class bitMap32 : public GraphicElement
{
public:
    bitMap32();
    virtual ~bitMap32();
    void Paint() override;
    uint8_t (*data)[128];
};


class bitMap48 : public GraphicElement
{
public:
    bitMap48();
    virtual ~bitMap48();
    void Paint() override;
    // 48 rows * 6 bytes per row = 288 bytes
    uint8_t (*data)[288];
};

#endif // GRAPHIC_ELEMENT_H
