#include <Arduino.h>
#include <Wire.h>
#include "modules/Task5_Modules/DHT20/DHT20.h"
#include "modules/Task5_Modules/tinyml.h"

DHT20 dht20;

// Globals, for the convenience of one-shot setup.
namespace
{
    tflite::ErrorReporter *error_reporter = nullptr;
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;
    constexpr int kTensorArenaSize = 8 * 1024; //
    uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setupTinyML()
{
    Serial.println("TensorFlow Lite Init....");
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

        // Initialize I2C and DHT20 (Yolo UNO: SDA=11, SCL=12 for I2C4 port)
    Wire.begin(11, 12);
    if (dht20.begin()) {
      Serial.println("DHT20 initialized successfully.");
    } else {
      Serial.println("DHT20 initialization failed!");
    }

    model = tflite::GetModel(dht_anomaly_model_tflite); // g_model_data is from model_data.h
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        error_reporter->Report("Model provided is schema version %d, not equal to supported version %d.",
                               model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        error_reporter->Report("AllocateTensors() failed");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    Serial.println("TensorFlow Lite Micro initialized on ESP32.");
}

void tiny_ml_task(void *pvParameters)
{

    setupTinyML();

    while (1)
    {
        // Read from DHT20
        int read_status = dht20.read();
        if (read_status != 0) {
            Serial.println("DHT20 read failed; retrying...");
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        float temperature = dht20.getTemperature();
        float humidity    = dht20.getHumidity();
        input->data.f[0] = temperature;
        input->data.f[1] = humidity;

        // Run inference
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk)
        {
            error_reporter->Report("Invoke failed");
            return;
        }

        // Get and process output
        float result = output->data.f[0];
        Serial.print("Inference result: ");
        Serial.println(result);
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C, Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// forward-declare the task implemented in task5_TinyML.cpp
void tiny_ml_task(void *pvParameters);

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  // create the TinyML task (adjust stack size/prio as needed)
  xTaskCreatePinnedToCore(tiny_ml_task, "TinyML", 8 * 1024, NULL, 1, NULL, 1);
}

void loop() {
  // keep Arduino loop idle; FreeRTOS tasks run the work
  vTaskDelay(portMAX_DELAY);
}