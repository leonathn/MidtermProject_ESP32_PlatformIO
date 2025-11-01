#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../config/config.h"
#include "../config/system_types.h"
#include "../ml/tinyml.h"

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
    Serial.println("[TinyML] Initializing TensorFlow Lite Micro");
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

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

    Serial.println("[TinyML] TensorFlow Lite Micro ready");
}

void tiny_ml_task(void *pvParameters)
{

    setupTinyML();

    if (input == nullptr || output == nullptr)
    {
        Serial.println("[TinyML] Initialization failed, deleting task");
        vTaskDelete(nullptr);
        return;
    }

    while (1)
    {
        // Use latest readings populated by Task 1
        float temperature = gLive.tC;
        float humidity    = gLive.rh;

        if (isnan(temperature) || isnan(humidity))
        {
            vTaskDelay(pdMS_TO_TICKS(TINYML_WAIT_FOR_DATA_MS));
            continue;
        }

        input->data.f[0] = temperature;
        input->data.f[1] = humidity;

        // Run inference
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk)
        {
            error_reporter->Report("Invoke failed");
            vTaskDelay(pdMS_TO_TICKS(TINYML_RETRY_DELAY_MS));
            continue;
        }

        // Get and process output
        float result = output->data.f[0];
        Serial.printf("[TinyML] Score %.3f (T=%.1f°C H=%.1f%%)\n", result, temperature, humidity);

        gLive.tinyml_score = result;
        gLive.tinyml_last_ms = millis();
        gLive.tinyml_runs++;

        vTaskDelay(pdMS_TO_TICKS(TINYML_INFERENCE_MS));
    }
}

void createTinyMLTask()
{
    BaseType_t created = xTaskCreatePinnedToCore(
        tiny_ml_task,
        "TinyML",
        TASK_TINYML_STACK_SIZE,
        nullptr,
        TASK_TINYML_PRIORITY,
        nullptr,
        APP_CPU_NUM);
    if (created != pdPASS)
    {
        Serial.println("[TinyML] Failed to create TinyML task");
    }
    else
    {
        Serial.println("[TinyML] Task created");
    }
}
