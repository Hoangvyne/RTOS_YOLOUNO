#include "led_RGB.h"

// Khởi tạo LED màu chân D3 (Trên Yolo Uno thường là chân GPIO 18 hoặc 48 tùy phiên bản, D3 là alias)
Adafruit_NeoPixel rgb(4, 3, NEO_GRB + NEO_KHZ800);

void TaskLed_RGB(void *pvParameters) {
    rgb.begin();
    rgb.setBrightness(50); // Đặt độ sáng vừa phải (0-255)

    while (1) {
        if (isAlert) {
            // --- TRẠNG THÁI CẢNH BÁO: Chớp đỏ ---
            for(int i=0; i<4; i++) rgb.setPixelColor(i, rgb.Color(255, 0, 0)); // Màu đỏ
            rgb.show();
            vTaskDelay(pdMS_TO_TICKS(300)); // Chớp nhanh 0.3s

            for(int i=0; i<4; i++) rgb.setPixelColor(i, rgb.Color(0, 0, 0)); // Tắt
            rgb.show();
            vTaskDelay(pdMS_TO_TICKS(300));
        } 
        else {
            
            for(int i=0; i<4; i++) rgb.setPixelColor(i, rgb.Color(0, 255, 0)); 
            rgb.show();
            
            vTaskDelay(pdMS_TO_TICKS(1000)); // Kiểm tra lại sau mỗi 1 giây
        }
    }
}