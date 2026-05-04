#include "global.h"

// ================== QUEUE ==================
QueueHandle_t sensorQueue = NULL;
QueueHandle_t ledQueue = NULL;
QueueHandle_t lcdQueue = NULL;
// ================== SEMAPHORE ==================
SemaphoreHandle_t internetSemaphore = NULL;
SemaphoreHandle_t alertSemaphore = NULL;
SemaphoreHandle_t sensorSemaphore = NULL;
// ================== INIT ==================
void initRTOSObjects()
{
    // Queue sensor (5 phần tử)
    sensorQueue = xQueueCreate(5, sizeof(SensorData_t));
    // Queue LCD
    lcdQueue = xQueueCreate(5, sizeof(LCDData_t));

    // Queue LED
    ledQueue = xQueueCreate(5, sizeof(LedCommand_t));

    // Binary semaphore
    internetSemaphore = xSemaphoreCreateBinary();
    alertSemaphore = xSemaphoreCreateBinary();
    sensorSemaphore = xSemaphoreCreateBinary();
    // Kiểm tra lỗi
    if (sensorQueue == NULL || ledQueue == NULL ||
        internetSemaphore == NULL || alertSemaphore == NULL)
    {
        Serial.println("Error creating RTOS objects!");
        while (1); // treo hệ thống nếu lỗi
    }
}