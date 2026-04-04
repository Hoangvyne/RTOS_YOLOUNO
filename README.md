Smart Environment Monitor with TinyML & IoT (Yolo Uno)
Dự án giám sát môi trường thông minh sử dụng mạch Yolo Uno (ESP32-S3), kết hợp hệ điều hành thời gian thực FreeRTOS, trí tuệ nhân tạo TinyML (TensorFlow Lite) và kết nối IoT (ThingsBoard).
🌟 Tính năng chính
Giám sát thời gian thực: Đọc nhiệt độ, độ ẩm (DHT20) và ánh sáng (LDR).
Hiển thị tại chỗ: Cập nhật dữ liệu liên tục lên màn hình LCD 1602 I2C.
Cảnh báo thông minh: Đèn NeoPixel (RGB) thay đổi hiệu ứng dựa trên trạng thái hệ thống và mức độ nguy hiểm (isAlert).
Trí tuệ nhân tạo (TinyML): Sử dụng mô hình TensorFlow Lite Micro để dự đoán trạng thái môi trường (Bình thường, Cần bật quạt, Cần bật đèn, Cảnh báo nguy hiểm).
Kết nối IoT: Đẩy dữ liệu lên Dashboard ThingsBoard và nhận lệnh điều khiển từ xa (RPC).
Đa nhiệm (Multitasking): Sử dụng FreeRTOS để quản lý các Task độc lập, đảm bảo tính ổn định và thời gian thực.
🛠 Phần cứng sử dụng
Board: Yolo Uno (ESP32-S3)
Cảm biến:
DHT20 (I2C) - Nhiệt độ & Độ ẩm
Quang trở LDR (Analog) - Ánh sáng
Hiển thị: LCD 1602 với module I2C (Địa chỉ 0x27/0x3F)
Đèn báo: NeoPixel tích hợp (GPIO 48)
📌 Sơ đồ kết nối (Pinout)
Linh kiện	Chân ESP32-S3	Cổng trên Yolo Uno
LCD 1602 (SDA)	GPIO 11	Cổng I2C
LCD 1602 (SCL)	GPIO 12	Cổng I2C
DHT20 (SDA)	GPIO 11	Cổng I2C
DHT20 (SCL)	GPIO 12	Cổng I2C
Light Sensor	GPIO 4	Cổng Analog
NeoPixel LED	GPIO 48	Tích hợp trên board
🏗 Cấu trúc mã nguồn
Dự án được tổ chức theo kiến trúc hướng đối tượng và chia nhỏ theo Task:
global.h / .cpp: Quản lý các biến toàn cục (glob_temperature, isAlert,...) và cơ chế đồng bộ (Mutex).
temp_humi.cpp: Task đọc cảm biến, xử lý logic báo động và cập nhật LCD.
neo_blinky.cpp: Task điều khiển hiệu ứng đèn RGB (Pulse, Wipe, Alert).
tinyml.cpp: Task chạy suy luận AI (Inference) mỗi 5 giây.
main.cpp: Khởi tạo hệ thống, kết nối WiFi/IoT và quản lý vòng đời các Task.
⚙️ Cài đặt & Sử dụng
Yêu cầu: Đã cài đặt VS Code và Extension PlatformIO.
Thư viện: Đảm bảo platformio.ini đã khai báo đầy đủ:
robtillaart/DHT20
marcoschwartz/LiquidCrystal_I2C
adafruit/Adafruit NeoPixel
tanakamasayuki/TensorFlowLite_ESP32
thingsboard/ThingsBoard
Cấu hình: Thay đổi WIFI_SSID, WIFI_PASSWORD và TOKEN ThingsBoard trong file code tương ứng.
Biên dịch: Nhấn nút Build (✓) và Upload (→) để nạp code vào Yolo Uno.
🧠 TinyML Logic
Mô hình AI nhận 3 đầu vào: Nhiệt độ (chuẩn hóa /40), Độ ẩm (chuẩn hóa /100) và Ánh sáng (chuẩn hóa /4095).
Các nhãn dự đoán bao gồm:
0: Normal (Môi trường lý tưởng)
1: Fan ON (Nhiệt độ quá cao)
2: Light ON (Thiếu ánh sáng)
3: Warning! (Môi trường khắc nghiệt)