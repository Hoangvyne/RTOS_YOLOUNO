#include "global.h"

#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi.h"
#include "tinyml.h"
#include "main_server2.h"
#include "core_iot.h"
void setup()
{
  Serial.begin(115200);

  xTaskCreate(TaskLed_blinky, "Task LED Blink", 2048, NULL, 2, NULL);
  xTaskCreate(TaskNeo_blinky, "Task Neo Blink", 4096, NULL, 1, NULL);
  xTaskCreate(TaskTempHumi, "Task Temp Humi", 4096, NULL, 1, NULL);
  xTaskCreate(main_server2_task, "Task Main Server", 8192, NULL, 1, NULL);
  xTaskCreate(coreiot_task, "Task Core IoT", 8192, NULL, 1, NULL); 
  xTaskCreate(tiny_ml_task, "Task TinyML", 16348, NULL, 1, NULL);
}

void loop()
{
} 