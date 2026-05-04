#include "neo_blinky.h"
#include "global.h"

void TaskNeo_blinky(void *pvParameters)
{
    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.show();

    LedCommand_t cmd;

    while (1)
    {
        if (xQueueReceive(ledQueue, &cmd, portMAX_DELAY))
        {
            // ===== PRIORITY: MANUAL CONTROL =====
            if (cmd.led2)
            {
                // User bật → màu trắng
                strip.setPixelColor(0, strip.Color(255, 255, 255));
            }
            else
            {
                // ===== AUTO MODE (TinyML) =====
                switch (cmd.status)
                {
                case STATUS_NORMAL:
                    strip.setPixelColor(0, strip.Color(0, 50, 0)); // xanh lá
                    break;

                case STATUS_HOT:
                    strip.setPixelColor(0, strip.Color(255, 0, 0)); // đỏ
                    break;

                case STATUS_HUMI:
                    strip.setPixelColor(0, strip.Color(0, 0, 255)); // xanh dương
                    break;

                case STATUS_ALERT:
                    strip.setPixelColor(0, strip.Color(255, 0, 0)); // đỏ (có thể blink)
                    break;

                default:
                    strip.clear();
                    break;
                }
            }

            strip.show();
        }
    }
}