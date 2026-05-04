#include "main_server2.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>

// ===== LOCAL CACHE (KHÔNG PHẢI GLOBAL HỆ THỐNG) =====
SensorData_t latestSensorData = {0};
bool led1_state = false;
bool led2_state = false;

bool isAPMode = true;
bool connecting = false;
unsigned long connect_start_ms = 0;

String wifi_ssid = "YoloUNO_Setup";
String wifi_password = "12345678";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// ===== CONTENT TYPE =====
String getContentType(String filename)
{
    if (filename.endsWith(".html"))
        return "text/html";
    if (filename.endsWith(".css"))
        return "text/css";
    if (filename.endsWith(".js"))
        return "application/javascript";
    return "text/plain";
}

// ===== STATIC FILE =====
void handleStaticFiles()
{
    String path = server.uri();
    if (path == "/")
        path = "/index.html";

    if (LittleFS.exists(path))
    {
        File file = LittleFS.open(path, "r");
        server.streamFile(file, getContentType(path));
        file.close();
    }
    else
    {
        server.send(404, "text/plain", "File not found");
    }
}

// ===== WEBSOCKET =====
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    if (type != WStype_TEXT)
        return;

    StaticJsonDocument<256> doc;
    if (deserializeJson(doc, payload))
        return;

    String page = doc["page"];

    // ===== DEVICE CONTROL =====
    if (page == "device")
    {
        String status = doc["value"]["status"];
        int gpio = doc["value"]["gpio"];

        bool state = (status == "ON");

        LedCommand_t cmd;
        cmd.led1 = led1_state;
        cmd.led2 = led2_state;

        if (gpio == LED1_PIN)
        {
            cmd.led1 = state;
            led1_state = state;
        }
        else if (gpio == LED2_PIN)
        {
            cmd.led2 = state;
            led2_state = state;
        }

        // Gửi sang LED task
        xQueueSend(ledQueue, &cmd, portMAX_DELAY);
    }

    // ===== WIFI CONFIG =====
    else if (page == "setting")
    {
        wifi_ssid = doc["value"]["ssid"].as<String>();
        wifi_password = doc["value"]["password"].as<String>();

        WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

        connecting = true;
        connect_start_ms = millis();

        webSocket.sendTXT(num, "{\"status\":\"connecting\"}");
    }
}

// ===== SENSOR API =====
void handleSensors()
{
    StaticJsonDocument<200> doc;

    doc["temp"] = latestSensorData.temperature;
    doc["hum"] = latestSensorData.humidity;
    doc["light"] = latestSensorData.light;

    doc["led1"] = led1_state ? "ON" : "OFF";
    doc["led2"] = led2_state ? "ON" : "OFF";

    String json;
    serializeJson(doc, json);

    server.send(200, "application/json", json);
}

// ===== SETUP SERVER =====
void setupServerRoutes()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS Mount Failed");
        return;
    }

    server.on("/sensors", HTTP_GET, handleSensors);
    server.onNotFound(handleStaticFiles);

    server.begin();
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);

    Serial.println("Server started");
}

// ===== AP MODE =====
void startAP()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(wifi_ssid.c_str(), wifi_password.c_str());
    Serial.println(WiFi.softAPIP());
}

// ===== BROADCAST =====
void broadcastData()
{
    StaticJsonDocument<200> doc;

    doc["temp"] = latestSensorData.temperature;
    doc["hum"] = latestSensorData.humidity;
    doc["light"] = latestSensorData.light;

    doc["led1"] = led1_state ? "ON" : "OFF";
    doc["led2"] = led2_state ? "ON" : "OFF";

    String json;
    serializeJson(doc, json);

    webSocket.broadcastTXT(json);
}

// ===== MAIN TASK =====
void main_server2_task(void *pvParameters)
{
    delay(3000);

    WiFi.mode(WIFI_AP_STA);
    startAP();
    setupServerRoutes();

    unsigned long lastBroadcast = 0;

    while (1)
    {
        server.handleClient();
        webSocket.loop();

        // ===== NHẬN DATA TỪ SENSOR TASK =====
        SensorData_t data;
        if (xQueueReceive(sensorQueue, &data, 0))
        {
            latestSensorData = data;
        }

        // ===== WIFI CONNECT =====
        if (connecting)
        {
            if (WiFi.status() == WL_CONNECTED)
            {
                connecting = false;
                webSocket.broadcastTXT("{\"status\":\"online\"}");
            }
            else if (millis() - connect_start_ms > 15000)
            {
                startAP();
                connecting = false;
                webSocket.broadcastTXT("{\"status\":\"error\"}");
            }
        }

        // ===== BROADCAST =====
        if (millis() - lastBroadcast > 2000)
        {
            broadcastData();
            lastBroadcast = millis();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}