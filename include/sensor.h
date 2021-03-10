#pragma once

#include <Arduino.h>

void sensorInit();
void sensorSetAlertState (boolean state);
int sensorReadGasLevel ();