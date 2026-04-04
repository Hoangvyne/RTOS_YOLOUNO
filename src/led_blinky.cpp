#include "led_blinky.h"


void TaskLed_blinky(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);
    int ledState = 0;

    while (1) {
        if (ledState == 0) {
            digitalWrite(LED_PIN, HIGH);
        } else {
            digitalWrite(LED_PIN, LOW);
        }

        ledState = 1 - ledState;

        // Delay 2 giây (chuẩn RTOS)
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}