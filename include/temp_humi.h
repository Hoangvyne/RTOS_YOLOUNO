#ifndef TEMP_HUMI_H
#define TEMP_HUMI_H

#include <Arduino.h>
#include "global.h"
#include "DHT20.h"
#include "LiquidCrystal_I2C.h"

void TaskTempHumi(void *pvParameters);

#endif