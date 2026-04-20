#include "main_server.h"
#include <WiFi.h>
#include <WebServer.h>

bool led1_state = false;
bool led2_state = false;
bool isAPMode = true;

WebServer server(80);


String wifi_ssid = "ESP32_DASHBOARD";
String wifi_password = "";

unsigned long connect_start_ms = 0;
bool connecting = false;

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>ESP32 Dashboard</title>
  <style>
    body { font-family: Arial; text-align:center; margin:0; background:#f4f4f4;}
    .container { margin:20px auto; max-width:350px; background:#fff; border-radius:10px; box-shadow:0 2px 10px #ccc; padding:20px;}
    .sensor-box { text-align:left; background:#e9ecef; padding:15px; border-radius:8px; margin-bottom:15px; line-height:1.6;}
    button { padding:12px; margin:10px 5px; font-size:16px; width:45%; cursor:pointer; border-radius:5px; border:1px solid #ddd;}
    #settings { width:93%; background:#007bff; color:white; border:none; margin-top:15px;}
  </style>
</head>
<body>
  <div class='container'>
    <h2>ESP32 Monitor</h2>
    <div class='sensor-box'>
      <b>🌡️ Nhiệt độ:</b> <span id='temp'>--</span> °C<br>
      <b>💧 Độ ẩm:</b> <span id='hum'>--</span> %<br>
      <b>☀️ Ánh sáng:</b> <span id='light'>--</span> %
    </div>
    <div>
        <button onclick='toggleLED(1)'>LED 1: <span id="l1">...</span></button>
        <button onclick='toggleLED(2)'>LED 2: <span id="l2">...</span></button>
    </div>
    <button id="settings" onclick="window.location='/settings'">⚙️ Cài đặt WiFi</button>
  </div>
  <script>
    function updateData() {
      fetch('/sensors').then(res => res.json()).then(d => {
         document.getElementById('temp').innerText = d.temp;
         document.getElementById('hum').innerText = d.hum;
         document.getElementById('light').innerText = d.light;
         document.getElementById('l1').innerText = d.led1;
         document.getElementById('l2').innerText = d.led2;
      });
    }
    function toggleLED(id) {
      fetch('/toggle?led=' + id).then(res => res.json()).then(updateData);
    }
    setInterval(updateData, 2000);
    window.onload = updateData;
  </script>
</body></html>
)rawliteral";

const char SETTINGS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>WiFi Settings</title>
  <style>
    body { font-family: Arial; text-align:center; margin:0; background:#f4f4f4;}
    .container { margin:20px auto; max-width:350px; background:#fff; border-radius:10px; padding:20px; box-shadow:0 2px 10px #ccc;}
    input { width:90%; padding:10px; margin:10px 0; border:1px solid #ccc; border-radius:5px;}
    button { padding:12px; width:46%; cursor:pointer; font-size:16px; border-radius:5px; border:none;}
    .btn-save { background:#28a745; color:white;}
    .btn-back { background:#6c757d; color:white;}
  </style>
</head>
<body>
  <div class='container'>
    <h2>Cấu hình WiFi</h2>
    <form id="wifiForm">
      <input name="ssid" id="ssid" placeholder="Tên WiFi (SSID)" required>
      <input name="pass" id="pass" type="password" placeholder="Mật khẩu">
      <div style="margin-top:10px;">
        <button type="submit" class="btn-save">Kết nối</button>
        <button type="button" class="btn-back" onclick="window.location='/'">Quay lại</button>
      </div>
    </form>
    <p id="msg"></p>
  </div>
  <script>
    document.getElementById('wifiForm').onsubmit = function(e){
      e.preventDefault();
      let s = document.getElementById('ssid').value;
      let p = document.getElementById('pass').value;
      document.getElementById('msg').innerText = "Đang gửi yêu cầu...";
      fetch('/connect?ssid='+encodeURIComponent(s)+'&pass='+encodeURIComponent(p))
      .then(r=>r.text()).then(m => { document.getElementById('msg').innerText = m; });
    };
  </script>
</body></html>
)rawliteral";

void handleRoot() {
    server.send_P(200, "text/html", INDEX_HTML); 
}

void handleSensors() {
  String json = "{";
  json += "\"temp\":" + String(glob_temperature, 1) + ",";
  json += "\"hum\":" + String(glob_humidity, 1) + ",";
  json += "\"light\":" + String(glob_light, 1) + ","; 
  json += "\"led1\":\"" + String(led1_state ? "ON" : "OFF") + "\",";
  json += "\"led2\":\"" + String(led2_state ? "ON" : "OFF") + "\"";
  json += "}";
  server.send(200, "application/json", json);
}


void handleToggle() {
    int led = server.arg("led").toInt();
    if (led == 1) {
        led1_state = !led1_state;
        glob_led1_state = led1_state;
    } else if (led == 2) {
        led2_state = !led2_state;
        glob_led2_state = led2_state;
    }
    handleSensors(); // Trả về trạng thái mới ngay lập tức
}

void handleSettings() { server.send(200, "text/html", SETTINGS_HTML); }

void handleConnect()
{
  /* wifi_ssid = server.arg("ssid");
  wifi_password = server.arg("pass");
  server.send(200, "text/plain", "Connecting....");
  isAPMode = false;
  connecting = true;
  connect_start_ms = millis();
  connectToWiFi(); */
   String new_ssid = server.arg("ssid");
    String new_pass = server.arg("pass");
    
    // Gửi phản hồi về trình duyệt TRƯỚC khi thực hiện kết nối
    server.send(200, "text/plain", "Đang thử kết nối tới " + new_ssid + ". Vui lòng đợi 10s...");
    
    // Bắt đầu quá trình kết nối
    WiFi.begin(new_ssid.c_str(), new_pass.c_str());
    Serial.printf("\nConnecting to %s...\n", new_ssid.c_str());
    
    connecting = true;
    connect_start_ms = millis();
}

// Hàm khởi tạo server (chỉ gọi 1 lần trong setup)
void setupServerRoutes() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/toggle", HTTP_GET, handleToggle);
    server.on("/sensors", HTTP_GET, handleSensors);
    server.on("/settings", HTTP_GET, handleSettings); // Cần giữ nguyên hàm handleSettings của bạn
    server.on("/connect", HTTP_GET, handleConnect);
    server.begin();
}

void startAP()
{
  /* WiFi.mode(WIFI_AP);
  WiFi.softAP(wifi_ssid.c_str(), wifi_password.c_str());
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  isAPMode = true;
  connecting = false; */
  WiFi.mode(WIFI_AP_STA); 
    WiFi.softAP(wifi_ssid.c_str(), wifi_password.c_str());
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    isAPMode = true;
    connecting = false;
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  Serial.print("Connecting to ");
  Serial.print(wifi_ssid.c_str());
  Serial.print(wifi_password.c_str());

  Serial.println(wifi_ssid);
}

void main_server_task(void *pvParameters) {
    pinMode(BOOT_PIN, INPUT_PULLUP);
    
    startAP(); 
    setupServerRoutes(); // Đăng ký route 1 lần

    while (1) {
        server.handleClient();

        // Kiểm tra nút BOOT để Reset về AP Mode
        if (digitalRead(BOOT_PIN) == LOW) {
            vTaskDelay(pdMS_TO_TICKS(500)); // Debounce lâu hơn một chút
            if (digitalRead(BOOT_PIN) == LOW) {
                Serial.println("Force AP Mode...");
                startAP();
            }
        }

        // Logic kết nối WiFi
        if (connecting) {
            if (WiFi.status() == WL_CONNECTED) {
                Serial.print("STA Connected! IP: ");
                Serial.println(WiFi.localIP());
                isWifiConnected = true;
                isAPMode = false;
                connecting = false;
                // Có thể tắt AP ở đây để tiết kiệm điện nếu muốn: WiFi.mode(WIFI_STA);
            } 
            else if (millis() - connect_start_ms > 15000) { // Tăng lên 15s cho chắc chắn
                Serial.println("WiFi Fail! Back to AP.");
                startAP();
                connecting = false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}