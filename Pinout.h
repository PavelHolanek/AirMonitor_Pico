#ifndef PINOUT_H
#define PINOUT_H

// Power switching MOSFETs
#define MOSFET_3V3_PIN 16
#define MOSFET_5V_PIN 17

// Display, ILI9488 over SPI. 255 means the signal is not wired up.
#define TFT_SCLK 6
#define TFT_MOSI 7
#define TFT_MISO 255
#define TFT_CS 13
#define TFT_RST 14
#define TFT_DC 15
#define TFT_BACKLIGHT 255

// Joystick
#define GPIO_PUSH_PIN 3
#define GPIO_MOVE_PIN 9
#define ADC0_PIN 26
#define ADC1_PIN 27

// DS3231 real time clock
#define SDA_PIN_CLOCK 18
#define SDL_PIN_CLOCK 19

// Sensor I2C bus, shared by all three sensors
#define SENSORS_I2C_SDA_PIN 20
#define SENSORS_I2C_SCL_PIN 21

#endif // PINOUT_H
