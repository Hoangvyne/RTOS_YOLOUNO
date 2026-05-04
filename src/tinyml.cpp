#include "tinyml.h"
#include "global.h"

#define TEMP_MIN 20.0f
#define TEMP_MAX 40.0f
#define HUMI_MIN 0.0f
#define HUMI_MAX 100.0f

float normalize(float x, float min, float max)
{
    return (x - min) / (max - min);
}

// ===== TFLITE =====
namespace
{
    tflite::ErrorReporter *tf_error_reporter = nullptr;
    const tflite::Model *tf_model = nullptr;
    tflite::MicroInterpreter *tf_interpreter = nullptr;
    TfLiteTensor *tf_input = nullptr;
    TfLiteTensor *tf_output = nullptr;

    constexpr int kTensorArenaSize = 16 * 1024;
    uint8_t tensor_arena[kTensorArenaSize];
}

// ===== SETUP =====
void setupTinyML()
{
    Serial.println(">>> TinyML Init");

    static tflite::MicroErrorReporter micro_error_reporter;
    tf_error_reporter = &micro_error_reporter;

    tf_model = tflite::GetModel(smart_env_model_tflite);

    if (tf_model->version() != TFLITE_SCHEMA_VERSION)
    {
        Serial.println("Model mismatch!");
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        tf_model, resolver, tensor_arena, kTensorArenaSize, tf_error_reporter);

    tf_interpreter = &static_interpreter;

    if (tf_interpreter->AllocateTensors() != kTfLiteOk)
    {
        Serial.println("Allocate failed");
        return;
    }

    tf_input = tf_interpreter->input(0);
    tf_output = tf_interpreter->output(0);

    Serial.println(">>> TinyML Ready");
}

// ===== TASK =====
void tiny_ml_task(void *pvParameters)
{
    setupTinyML();

    SensorData_t data;
    LCDData_t lcdData;
    
    while (1)
    {
        // Đợi signal có data mới
        if (xSemaphoreTake(sensorSemaphore, portMAX_DELAY))
        {
            // Lấy data từ queue (đã được Control task đảm bảo)
            if (xQueueReceive(sensorQueue, &data, 0))
            {
                // ===== NORMALIZE =====
                tf_input->data.f[0] = normalize(data.temperature, TEMP_MIN, TEMP_MAX);
                tf_input->data.f[1] = normalize(data.humidity, HUMI_MIN, HUMI_MAX);

                unsigned long start = millis();

                if (tf_interpreter->Invoke() == kTfLiteOk)
                {
                    unsigned long end = millis();

                    float temp_score = tf_output->data.f[0];
                    float humi_score = tf_output->data.f[1];

                    int temp_pred = temp_score > 0.5f;
                    int humi_pred = humi_score > 0.5f;

                    // ===== LED CONTROL =====
                    LedCommand_t cmd;
                    cmd.led1 = temp_pred;
                    cmd.led2 = humi_pred;
                    cmd.status = STATUS_NORMAL;
                    if (temp_pred)
                    {
                        cmd.status = STATUS_HOT;
                    }
                    else if (humi_pred)
                    {
                        cmd.status = STATUS_HUMI;
                    }
                    if(data.isAlert)
                    {
                        cmd.status = STATUS_ALERT;
                    }
                    xQueueSend(ledQueue, &cmd, portMAX_DELAY);

                    // ===== LCD UPDATE =====
                    snprintf(lcdData.line2, sizeof(lcdData.line2), "STATUS:%s",
                    temp_pred ? "HOT" : (humi_pred ? "HUMI" : "NORMAL"));

                    xQueueSend(lcdQueue, &lcdData, portMAX_DELAY);
                    // ===== DEBUG =====
                    Serial.println("===== TinyML =====");
                    Serial.print("Inference: ");
                    Serial.print(end - start);
                    Serial.println(" ms");

                    Serial.print("Temp Score: ");
                    Serial.println(temp_score);

                    Serial.print("Humi Score: ");
                    Serial.println(humi_score);

                    Serial.print("Temp → ");
                    Serial.println(temp_pred ? "HOT" : "NORMAL");

                    Serial.print("Humi → ");
                    Serial.println(humi_pred ? "HIGH" : "NORMAL");
                }
            }
        }
    }
}