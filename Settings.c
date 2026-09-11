#include "Settings.h"

uint32_t sensorsMeassurementPeriod = 120000U;   // 2 min
uint32_t timeUpdatePeriod = 30000U;
uint32_t idleTime = 10000U;

uint16_t joystickCalibration0 = 2000;
uint16_t joystickCalibration1 = 2000;

GRAPH_ALGORITHM graphAlgorithm = GRAPH_ALGORITHM_BUCKET_AVERAGE;
INTERVAL meassurementInterval = INTERVAL_2_MIN;

// No correction until the user dials one in. Nothing applies these yet.
int32_t temperatureCalibration = 0;
int32_t preassureCalibration = 0;
int32_t humidityCalibration = 0;
int32_t co2Calibration = 0;

bool useFixedValuesSpanForGraph = false;
bool developerMode = false;
bool screenBacklightWhenIdle = true;
