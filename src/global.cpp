#include "global.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

float glob_temperature = 0.0;
float glob_humidity = 0.0;
float glob_light = 0.0;
boolean isAlert = false;
boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();
bool led_D3 = false;  // Khởi tạo giá trị ban đầu
bool led_NEO = false;
bool glob_led1_state = false; 
bool glob_led2_state = false;
QueueHandle_t sensorQueue;
SemaphoreHandle_t fanSemaphore;
SemaphoreHandle_t alertSemaphore;