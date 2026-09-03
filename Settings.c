#include "Settings.h"

uint32_t sensorsMeassurementPeriod = 120000U;   // 2 min
uint32_t timeUpdatePeriod = 30000U;
uint32_t idleTime = 10000U;

GRAPH_ALGORITHM graphAlgorithm = GRAPH_ALGORITHM_BUCKET_AVERAGE;
