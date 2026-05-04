#ifndef GLOBAL_H
#define GLOBAL_H

//STATUS
#define STATUS_NORMAL 0
#define STATUS_HOT    1
#define STATUS_HUMI   2
#define STATUS_ALERT  3

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// ================== STRUCT ==================

// Dữ liệu sensor
typedef struct {
    float temperature;
    float humidity;
    float light;
    bool isAlert;
} SensorData_t;

typedef struct {
    char line1[17]; // 16 chars + null terminator
    char line2[17];
} LCDData_t;
// Lệnh điều khiển LED
typedef struct {
    bool led1;
    bool led2;
    int status;
} LedCommand_t;

// ================== QUEUE ==================

// Queue truyền dữ liệu sensor
extern QueueHandle_t sensorQueue;

// Queue điều khiển LED
extern QueueHandle_t ledQueue;
// Queue cập nhật LCD
extern QueueHandle_t lcdQueue;

// ================== SEMAPHORE ==================

// Báo có kết nối internet
extern SemaphoreHandle_t internetSemaphore;

// Báo alert
extern SemaphoreHandle_t alertSemaphore;

// ================== INIT ==================
void initRTOSObjects();
extern SemaphoreHandle_t sensorSemaphore;
#endif