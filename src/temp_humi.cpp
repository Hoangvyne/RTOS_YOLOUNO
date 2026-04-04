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
  Serial.println("temp,humidity,light,label");
  while (1) {
    // 1. Đọc cảm biến DHT20
    int status = DHT.read();
    if (status == DHT20_OK) {
        glob_temperature = DHT.getTemperature();
        glob_humidity = DHT.getHumidity();
    }
    int rawLight = analogRead(LIGHT_PIN);
    glob_light = (rawLight / 4095.0) * 100.0; 

    int label;
    if (glob_temperature > 30) label = 1;
    else if (glob_light < 20) label = 2;
    else if (glob_humidity > 80) label = 3;
    else label = 0;

    if (glob_temperature > 32.0 || glob_humidity > 85.0) {
        isAlert = true; 
    } else {
        isAlert = false;
    }
    
    Serial.printf("%.1f,%.1f,%.1f,%d\n", 
                  glob_temperature, 
                  glob_humidity, 
                  glob_light,
                  label);

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