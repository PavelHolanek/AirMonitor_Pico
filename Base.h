#ifndef GRAPHIC_BASE_H
#define GRAPHIC_BASE_H


#include <stdint.h>

using Color = uint16_t;

typedef enum
{
    SENSOR_BMP280 = 0,
    SENSOR_SHT40,
    SENSOR_SDC41
} SENSOR;

typedef enum
{
    QUANTITY_TEMPERATURE = 0,
    QUANTITY_CO2,
    QUANTITY_HUMIDITY,
    QUANTITY_PRESSURE
} QUANTITY;

#endif //GRAPHIC_BASE_H
