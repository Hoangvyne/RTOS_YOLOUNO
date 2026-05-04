#include "temp_humi.h"
#include <Wire.h>

#define I2C_SDA 11
#define I2C_SCL 12

LiquidCrystal_I2C lcd(33, 16, 2);

void TaskTempHumi(void *pvParameters)
{
  Wire.begin(I2C_SDA, I2C_SCL);

  DHT20 DHT(&Wire);
  DHT.begin();

  lcd.init();
  lcd.backlight();
  lcd.print("System Ready!");
  vTaskDelay(pdMS_TO_TICKS(1000));

  SensorData_t data;
  LCDData_t lcdData;

  // mặc định status
  char statusLine[16] = "STATUS:NORMAL";

  while (1)
  {
    // ===== Đọc sensor =====
    int status = DHT.read();
    if (status == DHT20_OK)
    {
      data.temperature = DHT.getTemperature();
      data.humidity = DHT.getHumidity();
    }

    data.isAlert = (data.temperature > 32.0 || data.humidity > 85.0);

    // ===== Gửi sensor =====
    xQueueSend(sensorQueue, &data, portMAX_DELAY);

    // ===== Nhận status từ TinyML =====
    if (xQueueReceive(lcdQueue, &lcdData, 0))
    {
      strncpy(statusLine, lcdData.line2, sizeof(statusLine));
    }
    else
    {
      // fallback nếu không có TinyML
      strcpy(statusLine, data.isAlert ? "STATUS:ALERT" : "STATUS:NORMAL");
    }

    // ===== HIỂN THỊ LCD =====
    lcd.clear();

    // DÒNG 1: TEMP + HUMI
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(data.temperature, 1);
    lcd.print(" H:");
    lcd.print(data.humidity, 0);

    // DÒNG 2: STATUS
    lcd.setCursor(0, 1);
    lcd.print(statusLine);

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}