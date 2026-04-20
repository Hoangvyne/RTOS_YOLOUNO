#include "neo_blinky.h"
#include "global.h"
void TaskNeo_blinky(void *pvParameters) {
    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.show();
    while(1) {
        /* if (isAlert) {
            // NẾU BÁO ĐỘNG: Chớp tắt màu ĐỎ liên tục
            strip.setPixelColor(0, strip.Color(255, 0, 0)); // Đỏ rực
            strip.show();
            vTaskDelay(200 / portTICK_PERIOD_MS); // Chớp nhanh

            strip.setPixelColor(0, strip.Color(0, 0, 0)); // Tắt
            strip.show();
            vTaskDelay(200 / portTICK_PERIOD_MS);
        } else {
            // NẾU BÌNH THƯỜNG: Sáng nhẹ màu XANH LÁ (Green)
            strip.setPixelColor(0, strip.Color(0, 50, 0)); 
            strip.show();
            vTaskDelay(1000 / portTICK_PERIOD_MS); // Nghỉ lâu hơn
        } */
       if(glob_led2_state==true){
            strip.setPixelColor(0, strip.Color(255, 0, 0)); // Đỏ rực
            strip.show();
            
        } else {
            // NẾU BÌNH THƯỜNG: Sáng nhẹ màu XANH LÁ (Green)
            strip.clear();
            strip.show();
        }
        vTaskDelay(100); // Kiểm tra trạng thái mỗi 100ms
    }
}