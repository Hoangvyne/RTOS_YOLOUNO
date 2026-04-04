# 🌿 Smart Environment Monitor with TinyML & IoT (Yolo Uno)

Dự án giám sát môi trường thông minh sử dụng **Yolo Uno (ESP32-S3)**, kết hợp:

*  **FreeRTOS** (đa nhiệm thời gian thực)
*  **TinyML (TensorFlow Lite Micro)**
*  **IoT (ThingsBoard)**

---

##  Tính năng chính

*  **Giám sát thời gian thực**
  Đọc dữ liệu từ:

  * Nhiệt độ, độ ẩm (**DHT20**)
  * Ánh sáng (**LDR**)

*  **Hiển thị tại chỗ**
  Hiển thị dữ liệu lên **LCD 1602 I2C**

*  **Cảnh báo thông minh**
  Đèn **NeoPixel RGB** thay đổi trạng thái theo mức độ nguy hiểm (`isAlert`)

*  **Trí tuệ nhân tạo (TinyML)**
  Dự đoán trạng thái môi trường:

  * Normal
  * Fan ON
  * Light ON
  * Warning

*  **Kết nối IoT**

  * Gửi dữ liệu lên **ThingsBoard Dashboard**
  * Nhận lệnh điều khiển từ xa (RPC)

*  **Đa nhiệm (Multitasking)**
  Sử dụng **FreeRTOS** để chạy nhiều task độc lập

---

## 🛠 Phần cứng sử dụng

*  **Board**: Yolo Uno (ESP32-S3)

*  **Cảm biến**

  * DHT20 (I2C) – Nhiệt độ & Độ ẩm
  * LDR (Analog) – Ánh sáng

*  **Hiển thị**

  * LCD 1602 I2C (0x27 / 0x3F)

*  **Đèn**

  * NeoPixel RGB (GPIO 48)

---

## 📌 Sơ đồ kết nối (Pinout)

| Linh kiện   | ESP32-S3 | Cổng    |
| ----------- | -------- | ------- |
| LCD (SDA)   | GPIO 11  | I2C     |
| LCD (SCL)   | GPIO 12  | I2C     |
| DHT20 (SDA) | GPIO 11  | I2C     |
| DHT20 (SCL) | GPIO 12  | I2C     |
| LDR         | GPIO 4   | Analog  |
| NeoPixel    | GPIO 48  | Onboard |

---

## 🏗 Cấu trúc project

```
project/
│── src/
│   ├── main.cpp
│   ├── temp_humi.cpp
│   ├── neo_blinky.cpp
│   ├── tinyml.cpp
│   └── global.cpp
│
│── include/
│   └── global.h
│
│── platformio.ini
│── README.md
```

### 📂 Mô tả file

* `global.*` → Biến toàn cục + Mutex
* `temp_humi.cpp` → Đọc sensor + LCD + cảnh báo
* `neo_blinky.cpp` → Điều khiển LED RGB
* `tinyml.cpp` → AI inference mỗi 5s
* `main.cpp` → Khởi tạo hệ thống + WiFi + Task

---

## ⚙️ Cài đặt & sử dụng

### 🔧 Yêu cầu

* VSCode + PlatformIO

### 📚 Thư viện

```ini
robtillaart/DHT20
marcoschwartz/LiquidCrystal_I2C
adafruit/Adafruit NeoPixel
tanakamasayuki/TensorFlowLite_ESP32
thingsboard/ThingsBoard
```

