import serial
import csv
import time

# ===== CHỈNH PORT COM =====
SERIAL_PORT = "COM3"   # Windows: COM3, COM4...
BAUD_RATE = 115200

# ===== FILE CSV =====
FILE_NAME = "sensor_data.csv"

# Mở Serial
ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
time.sleep(2)  # chờ ESP32 reset

# Mở file CSV
with open(FILE_NAME, mode='w', newline='') as file:
    writer = csv.writer(file)

    # Ghi header
    writer.writerow(["temp", "humidity", "light", "label"])

    print("Đang đọc dữ liệu... Nhấn Ctrl+C để dừng\n")

    try:
        while True:
            line = ser.readline().decode('utf-8').strip()
            
            if line:
                print("Nhận:", line)

                # Tách dữ liệu
                parts = line.split(",")

                if len(parts) == 4:
                    writer.writerow(parts)
                    file.flush()  # ghi ngay xuống file

    except KeyboardInterrupt:
        print("\nĐã dừng và lưu file.")
        ser.close()