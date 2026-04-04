#include "tinyml.h"
#include "global.h" // Đảm bảo include để lấy glob_temperature, glob_humidity, glob_light

// Sử dụng tên biến cụ thể để tránh xung đột (Ambiguous)
namespace
{
    tflite::ErrorReporter *tf_error_reporter = nullptr;
    const tflite::Model *tf_model = nullptr; // Đổi từ model thành tf_model
    tflite::MicroInterpreter *tf_interpreter = nullptr;
    TfLiteTensor *tf_input = nullptr;
    TfLiteTensor *tf_output = nullptr;
    
    // Yolo Uno (ESP32-S3) có nhiều RAM, bạn có thể tăng nếu model lớn
    constexpr int kTensorArenaSize = 16 * 1024; 
    uint8_t tensor_arena[kTensorArenaSize];
}

void setupTinyML()
{
    Serial.println(">>> TensorFlow Lite Initializing...");
    static tflite::MicroErrorReporter micro_error_reporter;
    tf_error_reporter = &micro_error_reporter;

    // Đổi tên biến ở đây
    tf_model = tflite::GetModel(smart_env_model_tflite); 
    
    if (tf_model->version() != TFLITE_SCHEMA_VERSION)
    {
        tf_error_reporter->Report("Model version mismatch!");
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        tf_model, resolver, tensor_arena, kTensorArenaSize, tf_error_reporter);
    tf_interpreter = &static_interpreter;

    if (tf_interpreter->AllocateTensors() != kTfLiteOk)
    {
        tf_error_reporter->Report("AllocateTensors() failed");
        return;
    }

    tf_input = tf_interpreter->input(0);
    tf_output = tf_interpreter->output(0);

    Serial.println(">>> TinyML Initialized Successfully!");
}

void tiny_ml_task(void *pvParameters)
{
    setupTinyML();

    while (1)
    {
        if (tf_input != nullptr && tf_interpreter != nullptr) {
            
            // ===== ĐƯA DỮ LIỆU VÀO (Normalization) =====
            // Sử dụng các biến Global bạn đã đọc từ Task cảm biến
            tf_input->data.f[0] = glob_temperature / 40.0f;
            tf_input->data.f[1] = glob_humidity / 100.0f;
            tf_input->data.f[2] = glob_light / 4095.0f; // Nếu glob_light là raw analog (0-4095)

            // ===== CHẠY SUY LUẬN (Inference) =====
            if (tf_interpreter->Invoke() == kTfLiteOk)
            {
                // ===== ĐỌC KẾT QUẢ =====
                int predicted_label = 0;
                float max_val = tf_output->data.f[0];

                for (int i = 1; i < 4; i++)
                {
                    if (tf_output->data.f[i] > max_val)
                    {
                        max_val = tf_output->data.f[i];
                        predicted_label = i;
                    }
                }

                // ===== HÀNH ĐỘNG =====
                Serial.print("AI Prediction: ");
                switch (predicted_label)
                {
                    case 0: Serial.println("Normal"); break;
                    case 1: Serial.println("Need Fan (Hot)"); break;
                    case 2: Serial.println("Need Light (Dark)"); break;
                    case 3: Serial.println("ENV Warning!"); break;
                }
            }
        }

        // TinyML tốn tài nguyên, nên delay khoảng 5-10 giây mỗi lần đoán
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}