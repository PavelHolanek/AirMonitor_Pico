#include "Settings.h"

uint32_t timeUpdatePeriod = 30000U;
uint32_t idleTime = 10000U;

uint16_t joystickCalibration0 = 2000;
uint16_t joystickCalibration1 = 2000;

GRAPH_ALGORITHM graphAlgorithm = GRAPH_ALGORITHM_BUCKET_AVERAGE;
INTERVAL meassurementInterval = INTERVAL_2_MIN;

uint32_t intervalToMilliseconds(INTERVAL interval)
{
    switch (interval)
    {
    case INTERVAL_5_MIN:
        return 5U * 60U * 1000U;
    case INTERVAL_10_MIN:
        return 10U * 60U * 1000U;
    case INTERVAL_2_MIN:
    default:
        return 2U * 60U * 1000U;
    }
}

int32_t temperatureCalibration = 0;
int32_t preassureCalibration = 0;
int32_t humidityCalibration = 0;
int32_t co2Calibration = 0;

bool useFixedValuesSpanForGraph = false;
bool developerMode = false;

bool screenOffWhenIdle = false;

void settings_save(void)
{
    // TODO ESP32-S3 migration: persist the settings above into NVS
}
