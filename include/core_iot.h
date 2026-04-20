#ifndef CORE_IOT_H
#define CORE_IOT_H

#include <Arduino.h>
#include <WiFi.h>
#include "global.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>


void coreiot_task(void *pvParameters);

#endif