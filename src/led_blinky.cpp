#include "led_blinky.h"
#include "global.h"

void TaskLed_blinky(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);
    while (1) {
        /* if (ledState == 0) {
            digitalWrite(LED_PIN, HIGH);
        } else {
            digitalWrite(LED_PIN, LOW);
        }

        ledState = 1 - ledState;

        // Delay 2 giây (chuẩn RTOS)
        vTaskDelay(pdMS_TO_TICKS(2000)); */
        if(glob_led1_state==true){
            digitalWrite(LED_PIN, HIGH);
        } else {
            digitalWrite(LED_PIN, LOW);
        }
         vTaskDelay(pdMS_TO_TICKS(100)); // Kiểm tra trạng thái LED mỗi 100ms
    }
}