#include "temp_humi.h"
#include <Wire.h>
#include "global.h"

#define I2C_SDA 11
#define I2C_SCL 12
#define LIGHT_PIN 4 // Chân cảm biến ánh sáng trên Yolo Uno

LiquidCrystal_I2C lcd(0x27, 16, 2); 

void TaskTempHumi(void *pvParameters) {
  Wire.begin(I2C_SDA, I2C_SCL); 
  
  DHT20 DHT(&Wire);
  DHT.begin();

  lcd.init();
  lcd.backlight();
  lcd.print("System Ready!");
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  while (1) {
    // 1. Đọc cảm biến DHT20
    int status = DHT.read();
    if (status == DHT20_OK) {
        glob_temperature = DHT.getTemperature();
        glob_humidity = DHT.getHumidity();
    }

    // 2. Đọc cảm biến Ánh sáng (Analog)
    // Giá trị trả về từ 0 (tối) đến 4095 (sáng rực)
    int rawLight = analogRead(LIGHT_PIN);
    // Chuyển sang phần trăm (%) để dễ nhìn
    glob_light = (rawLight / 4095.0) * 100.0; 

    // 3. Kiểm tra báo động
    if (glob_temperature > 32.0 || glob_humidity > 85.0) {
        isAlert = true; 
    } else {
        isAlert = false;
    }

    // 4. In Serial Debug
    Serial.printf("T: %.1f, H: %.1f, L: %.1f%%\n", glob_temperature, glob_humidity, glob_light);

    // 5. Hiển thị LCD (Vì LCD chỉ có 2 dòng, mình sẽ ưu tiên hiển thị Light ở dòng 2)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(glob_temperature, 1);
    lcd.print("C H:"); lcd.print(glob_humidity, 0); lcd.print("%");

    lcd.setCursor(0, 1); 
    lcd.print("Light: ");
    lcd.print(glob_light, 1);
    lcd.print("%");

    vTaskDelay(2000 / portTICK_PERIOD_MS); 
  }
}