#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include "Base.h"
#include "Libraries/pico-displayDrivs/ili9488/ili9488.h" // Color struct definition

/*
  Parameters.h
  - Application-wide constants and compile-time parameters.
  - These are not meant to be changed at runtime by the user.
*/

constexpr uint16_t PARAM_SCREEN_WIDTH  = 480;
constexpr uint16_t PARAM_SCREEN_HEIGHT = 320;

// Common colors in 24-bit RGB (8-8-8)
constexpr Color PARAM_COLOR_BLACK = {0x00, 0x00, 0x00};
constexpr Color PARAM_COLOR_WHITE = {0xFF, 0xFF, 0xFF};
constexpr Color PARAM_COLOR_RED   = {0xFF, 0x00, 0x00};
constexpr Color PARAM_COLOR_GREEN = {0x00, 0xFF, 0x00};
constexpr Color PARAM_COLOR_BLUE  = {0x00, 0x00, 0xFF};

// Default UI / layout constants
constexpr uint8_t PARAM_DEFAULT_TEXT_SIZE = 2;
constexpr uint8_t PARAM_MAX_TEXT_SIZE = 6;

// Widget / layout defaults
constexpr uint16_t PARAM_DEFAULT_MARGIN = 6;
constexpr uint16_t PARAM_DEFAULT_PADDING = 3;

// Other compile-time parameters (placeholders — add as needed)
constexpr uint8_t PARAM_MAX_CHILDREN_PER_AREA = 4;

//MAIN WINDOW
constexpr uint8_t HEADER_HEIGHT = 75;
constexpr uint8_t HEADER_MARGIN = 12;

constexpr uint8_t BODY_HEIGHT = PARAM_SCREEN_HEIGHT - HEADER_HEIGHT;

constexpr uint8_t SENSOR_WIDGET_MARGIN = 12;
constexpr uint8_t SENSOR_WIDGET_PADDING = 12;
constexpr uint8_t SENSOR_WIDGET_HEIGHT = BODY_HEIGHT / 2 - 2 * SENSOR_WIDGET_MARGIN;
constexpr uint8_t SENSOR_WIDGET_WIDTH = PARAM_SCREEN_WIDTH / 2 - 2 * SENSOR_WIDGET_MARGIN;
constexpr uint8_t SENSOR_WIDGET_CORNER_RADIUS = 12;
constexpr uint8_t SENSOR_WIDGET_TEXT_SIZE = 4;

// Sensor-specific colors (currently green placeholders)
constexpr Color COLOR_TEMPERATURE = {0x00, 0xFF, 0x00};
constexpr Color COLOR_PRESSURE    = {0x00, 0xFF, 0x00};
constexpr Color COLOR_HUMIDITY    = {0x00, 0xFF, 0x00};
constexpr Color COLOR_CO2         = {0x00, 0xFF, 0x00};
#endif // PARAMETERS_H
