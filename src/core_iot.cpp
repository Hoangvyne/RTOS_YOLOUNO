#include "core_iot.h"
#include "global.h"
// ----------- CONFIGURE THESE! -----------
const char* coreIOT_Server = "app.coreiot.io";  
const char* coreIOT_Token = "hoangvy";   // Device Access Token
const int   mqttPort = 1883;
// ----------------------------------------

WiFiClient espClient;
PubSubClient client(espClient);


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect (username=token, password=empty)
    if (client.connect("ESP32Client", coreIOT_Token, NULL)) {
      Serial.println("connected to CoreIOT Server!");
      client.subscribe("v1/devices/me/rpc/request/+");
      Serial.println("Subscribed to v1/devices/me/rpc/request/+");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  // Allocate a temporary buffer for the message
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  Serial.print("Payload: ");
  Serial.println(message);

  // Parse JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* method = doc["method"];
  if (strcmp(method, "setStateLED") == 0) {
    // Check params type (could be boolean, int, or string according to your RPC)
    // Example: {"method": "setValueLED", "params": "ON"}
    const char* params = doc["params"];

    if (strcmp(params, "ON") == 0) {
      Serial.println("Device turned ON.");
      //TODO
      glob_led1_state = true;
        client.publish("v1/devices/me/attributes", "{\"ledStatus\":\"true\"}");
    } else {   
      Serial.println("Device turned OFF.");
      //TODO
      glob_led1_state = false;
        client.publish("v1/devices/me/attributes", "{\"ledStatus\":\"false\"}");
    }
  }
}


void setup_coreiot(){

  //Serial.print("Connecting to WiFi...");
  //WiFi.begin(wifi_ssid, wifi_password);
  //while (WiFi.status() != WL_CONNECTED) {
  
  // while (isWifiConnected == false) {
  //   delay(500);
  //   Serial.print(".");
  // }

  while(1){
    if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY)) {
      break;
    }
    delay(500);
    Serial.print(".");
  }


  Serial.println(" Connected!");

  client.setServer(coreIOT_Server, mqttPort);
  client.setCallback(callback);

}

void coreiot_task(void *pvParameters){
    setup_coreiot();
    xSemaphoreGive(xBinarySemaphoreInternet); 

    unsigned long lastPublish = 0;

    while(1){
        if (!client.connected()) {
            reconnect();
        }
        client.loop();
        if (millis() - lastPublish > 10000) {
            String payload = "{";
            payload += "\"temperature\":" + String(glob_temperature, 1) + ",";
            payload += "\"humidity\":" + String(glob_humidity, 1) + ",";
            payload += "\"light\":" + String(glob_light, 1);
            payload += "}";
            
            client.publish("v1/devices/me/telemetry", payload.c_str());
            Serial.println("Published payload: " + payload);
            
            lastPublish = millis(); // Cập nhật lại thời gian gửi cuối
        }
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}