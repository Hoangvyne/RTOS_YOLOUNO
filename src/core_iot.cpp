#include "core_iot.h"

// -------- CONFIG --------
const char *coreIOT_Server = "app.coreiot.io";
const char *coreIOT_Token = "hoangvy";
const int mqttPort = 1883;
// ------------------------

WiFiClient espClient;
PubSubClient client(espClient);

// ===== LOCAL CACHE =====
SensorData_t latestData = {0};

// ===== MQTT RECONNECT =====
void reconnect()
{
    while (!client.connected())
    {
        Serial.print("MQTT connecting...");

        if (client.connect("ESP32Client", coreIOT_Token, NULL))
        {
            Serial.println("connected!");
            client.subscribe("v1/devices/me/rpc/request/+");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.println(client.state());
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}

// ===== CALLBACK =====
void callback(char *topic, byte *payload, unsigned int length)
{
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';

    StaticJsonDocument<256> doc;
    if (deserializeJson(doc, message)) return;

    if (!doc.containsKey("method")) return;
        const char *method = doc["method"];

    if (strcmp(method, "setStateLED") == 0)
    {
        const char *params = doc["params"];

        bool state = (strcmp(params, "ON") == 0);

      LedCommand_t cmd;

// Chỉ set theo lệnh MQTT
cmd.led1 = state;
cmd.led2 = state; 

xQueueSend(ledQueue, &cmd, portMAX_DELAY);
        // Phản hồi attribute
        client.publish("v1/devices/me/attributes",
                       state ? "{\"ledStatus\":\"true\"}" : "{\"ledStatus\":\"false\"}");
    }
}

// ===== SETUP =====
void setup_coreiot()
{
    // Đợi WiFi sẵn sàng (từ task khác)
    xSemaphoreTake(internetSemaphore, portMAX_DELAY);

    Serial.println("WiFi Ready → MQTT start");

    client.setServer(coreIOT_Server, mqttPort);
    client.setCallback(callback);
}

// ===== MAIN TASK =====
void coreiot_task(void *pvParameters)
{
    setup_coreiot();

    unsigned long lastPublish = 0;

    while (1)
    {
        // ===== NHẬN SENSOR DATA =====
        SensorData_t data;
        if (xQueueReceive(sensorQueue, &data, 0))
        {
            latestData = data;
        }

        // ===== MQTT =====
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();

        // ===== PUBLISH =====
        if (millis() - lastPublish > 10000)
        {
            StaticJsonDocument<200> doc;

            doc["temperature"] = latestData.temperature;
            doc["humidity"] = latestData.humidity;
            doc["light"] = latestData.light;

            String payload;
            serializeJson(doc, payload);

            client.publish("v1/devices/me/telemetry", payload.c_str());

            Serial.println("MQTT Sent: " + payload);

            lastPublish = millis();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}