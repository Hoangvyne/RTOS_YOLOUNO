#ifndef GLOBAL_H
#define GLOBAL_H


#include <Arduino.h>


extern float glob_temperature;
extern float glob_humidity;
extern float glob_light;
extern boolean isAlert;
extern bool led_D3;
extern bool led_NEO;
extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet; 
extern bool glob_led1_state;
extern bool glob_led2_state; 
extern SemaphoreHandle_t fanSemaphore;
extern SemaphoreHandle_t alertSemaphore;
#endif