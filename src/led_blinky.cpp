#include "led_blinky.h"
#include "global.h"

void TaskLed_blinky(void *pvParameters)
{
    pinMode(LED_PIN, OUTPUT);

    LedCommand_t currentCmd;
    bool hasCmd = false;

    bool ledState = false;
    TickType_t lastToggle = 0;

    while (1)
    {
        // Nhận lệnh mới (không block lâu)
        if (xQueueReceive(ledQueue, &currentCmd, pdMS_TO_TICKS(50)))
        {
            hasCmd = true;
        }

        if (!hasCmd) continue;

        // ===== PRIORITY: USER CONTROL =====
        if (currentCmd.led1)
        {
            digitalWrite(LED_PIN, HIGH);
        }
        else
        {
            // ===== HEARTBEAT (nháy đều 500ms) =====
            if (xTaskGetTickCount() - lastToggle > pdMS_TO_TICKS(500))
            {
                ledState = !ledState;
                lastToggle = xTaskGetTickCount();
            }

            digitalWrite(LED_PIN, ledState);
        }
    }
}