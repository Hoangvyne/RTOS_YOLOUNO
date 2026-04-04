#include "temp_humi.h"
#include <Wire.h>
#include "global.h"

#define I2C_SDA 11
#define I2C_SCL 12

// Khởi tạo LCD
LiquidCrystal_I2C lcd(33,6, 2); 

void TaskTempHumi(void *pvParameters) {
  // 2. Khởi tạo I2C với chân cụ thể của Yolo Uno
  Wire.begin(I2C_SDA, I2C_SCL); 
  
  DHT20 DHT(&Wire);
  DHT.begin();

  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready!");
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  while (1) {
    int status = DHT.read();

    if (status == DHT20_OK) {
      glob_temperature = DHT.getTemperature();
      glob_humidity = DHT.getHumidity();

      if (glob_temperature > 32.0 || glob_humidity > 85.0) {
          isAlert = true; 
      } else {
          isAlert = false;
      }
      Serial.printf("Temp: %.1f C, Humi: %.1f %%\n", glob_temperature, glob_humidity);

      // Hiển thị LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(glob_temperature, 1);
      lcd.print(" C");

      lcd.setCursor(0, 1); 
      lcd.print("Humi: ");
      lcd.print(glob_humidity, 1);
      lcd.print(" %");

    } else {
      Serial.println("DHT20 Error!");
      lcd.setCursor(0, 0);
      lcd.print("Sensor Error!");
    }

    // Delay 2 giây giữa các lần đọc
    vTaskDelay(2000 / portTICK_PERIOD_MS); 
  }
}