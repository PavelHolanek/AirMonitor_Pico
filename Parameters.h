#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include "Base.h"

/*
  Parameters.h
  - Application-wide constants and compile-time parameters.
  - These are not meant to be changed at runtime by the user.
*/

constexpr uint16_t PARAM_SCREEN_WIDTH  = 320;
constexpr uint16_t PARAM_SCREEN_HEIGHT = 240;

// Common colors in RGB565
constexpr Color PARAM_COLOR_BLACK = 0x0000;
constexpr Color PARAM_COLOR_WHITE = 0xFFFF;
constexpr Color PARAM_COLOR_RED   = 0xF800;
constexpr Color PARAM_COLOR_GREEN = 0x07E0;
constexpr Color PARAM_COLOR_BLUE  = 0x001F;

// Default UI / layout constants
constexpr uint8_t PARAM_DEFAULT_TEXT_SIZE = 1;
constexpr uint8_t PARAM_MAX_TEXT_SIZE = 4;

// Widget / layout defaults
constexpr uint16_t PARAM_DEFAULT_MARGIN = 4;
constexpr uint16_t PARAM_DEFAULT_PADDING = 2;

// Other compile-time parameters (placeholders — add as needed)
constexpr uint8_t PARAM_MAX_CHILDREN_PER_AREA = 4;

//MAIN WINDOW
constexpr uint8_t HEADER_HEIGHT = 50;
constexpr uint8_t HEADER_MARGIN = 8;

constexpr uint8_t BODY_HEIGHT = PARAM_SCREEN_HEIGHT - HEADER_HEIGHT;

constexpr uint8_t SENSOR_WIDGET_MARGIN = 8;
constexpr uint8_t SENSOR_WIDGET_PADDING = 8;
constexpr uint8_t SENSOR_WIDGET_HEIGHT = BODY_HEIGHT / 2 - 2 * SENSOR_WIDGET_MARGIN;
constexpr uint8_t SENSOR_WIDGET_WIDTH = PARAM_SCREEN_WIDTH / 2 - 2 * SENSOR_WIDGET_MARGIN;
constexpr uint8_t SENSOR_WIDGET_CORNER_RADIUS = 8;
constexpr uint8_t SENSOR_WIDGET_TEXT_SIZE = 3;

constexpr Color COLOR_TEMPERATURE = 0x07E0;
constexpr Color COLOR_PRESSURE   = 0x07E0;
constexpr Color COLOR_HUMIDITY = 0x07E0;
constexpr Color COLOR_CO2  = 0x07E0;
#endif // PARAMETERS_H
