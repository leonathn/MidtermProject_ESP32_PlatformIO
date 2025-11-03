/**
 * @file task5_tinyml.cpp
 * @brief Task 6: TinyML Anomaly Detection using TensorFlow Lite Micro
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * This task implements on-device machine learning inference for anomaly detection
 * in temperature and humidity readings. It uses TensorFlow Lite Micro (TFLM) to
 * run a trained neural network model directly on the ESP32-S3 microcontroller.
 * 
 * Key Features:
 * - Real-time inference every 5 seconds
 * - Uses sensor data from Task 1 (DHT20)
 * - Outputs anomaly score (0.0 = normal, 1.0 = anomalous)
 * - Minimal memory footprint (8KB tensor arena)
 * - Independent task (no semaphore synchronization)
 * 
 * Model Details:
 * - Input: 2 floats (temperature, humidity)
 * - Output: 1 float (anomaly score)
 * - Model file: dht_anomaly_model.h (trained neural network)
 * - Framework: TensorFlow Lite Micro for embedded systems
 * 
 * Performance:
 * - Inference time: ~200-500ms per run
 * - Memory: 8KB tensor arena + model size
 * - Power: Minimal impact (runs at low priority)
 */

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../config/config.h"
#include "../config/system_types.h"
#include "../ml/tinyml.h"

/* ====== TensorFlow Lite Micro Components ====== */

/**
 * @brief Anonymous namespace for TinyML global objects
 * @details These objects maintain TensorFlow Lite Micro state across
 *          inference calls. They are initialized once and reused.
 * @note Using anonymous namespace prevents name collisions
 */
namespace
{
    /**
     * @brief TensorFlow Lite error reporter for debugging
     * @details Logs errors to serial console during inference
     */
    tflite::ErrorReporter *error_reporter = nullptr;
    
    /**
     * @brief Loaded neural network model
     * @details Parsed from dht_anomaly_model_tflite array
     */
    const tflite::Model *model = nullptr;
    
    /**
     * @brief TensorFlow Lite Micro interpreter
     * @details Executes the neural network forward pass
     */
    tflite::MicroInterpreter *interpreter = nullptr;
    
    /**
     * @brief Input tensor (temperature, humidity)
     * @details 2-element float array: [temperature, humidity]
     */
    TfLiteTensor *input = nullptr;
    
    /**
     * @brief Output tensor (anomaly score)
     * @details Single float: 0.0 (normal) to 1.0 (anomalous)
     */
    TfLiteTensor *output = nullptr;
    
    /**
     * @brief Tensor arena size in bytes
     * @details Working memory for intermediate activations and weights
     *          8KB is sufficient for small models (<10 layers)
     * @note Increase if model is complex or has many parameters
     */
    constexpr int kTensorArenaSize = 8 * 1024;  // 8 KB
    
    /**
     * @brief Tensor arena memory buffer
     * @details Static allocation ensures memory is not on stack
     *          This buffer holds all intermediate tensors during inference
     */
    uint8_t tensor_arena[kTensorArenaSize];
} // namespace

/* ====== TinyML Initialization ====== */

/**
 * @brief Initialize TensorFlow Lite Micro interpreter
 * @details Performs one-time setup:
 *          1. Create error reporter for debugging
 *          2. Load and validate neural network model
 *          3. Create operation resolver (registers ops like Dense, ReLU)
 *          4. Create interpreter with tensor arena
 *          5. Allocate tensors (input, output, intermediate)
 *          6. Get pointers to input/output tensors
 * 
 * @note This function is called once when TinyML task starts
 * @warning Failure here will cause task to self-destruct
 * @note Logs all steps to serial console for debugging
 */
void setupTinyML()
{
    Serial.println("[TinyML] Initializing TensorFlow Lite Micro");
    
    // Step 1: Create error reporter (logs to Serial)
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    // Step 2: Load neural network model from flash memory
    // Model is compiled into dht_anomaly_model.h as byte array
    model = tflite::GetModel(dht_anomaly_model_tflite);
    
    // Step 3: Verify model schema version
    // TFLITE_SCHEMA_VERSION = 3 (current version)
    // Mismatch indicates incompatible model format
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        error_reporter->Report("Model provided is schema version %d, not equal to supported version %d.",
                               model->version(), TFLITE_SCHEMA_VERSION);
        return;  // Fatal error - model incompatible
    }

    // Step 4: Create operation resolver
    // AllOpsResolver includes all TensorFlow Lite operations
    // For production, use MicroMutableOpResolver with only needed ops (saves memory)
    static tflite::AllOpsResolver resolver;
    
    // Step 5: Create interpreter
    // Interpreter executes the model using the provided tensor arena
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    // Step 6: Allocate tensors (input, output, intermediate activations)
    // This reserves memory in tensor_arena for all layers
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        error_reporter->Report("AllocateTensors() failed");
        return;  // Fatal error - not enough memory
    }

    // Step 7: Get pointers to input and output tensors
    // Input: 2 floats [temperature, humidity]
    // Output: 1 float [anomaly_score]
    input = interpreter->input(0);    // First (and only) input tensor
    output = interpreter->output(0);  // First (and only) output tensor

    Serial.println("[TinyML] TensorFlow Lite Micro ready");
}

/* ====== TinyML Task Function ====== */

/**
 * @brief FreeRTOS task for TinyML inference
 * @param pvParameters Unused (required by FreeRTOS)
 * @details Task behavior:
 *          1. Initialize TensorFlow Lite Micro (one-time setup)
 *          2. Loop forever:
 *             a. Wait for valid sensor data from Task 1
 *             b. Copy data into input tensor
 *             c. Run neural network inference
 *             d. Store output in global state
 *             e. Wait 5 seconds before next inference
 * 
 * @note Runs independently at Priority 1 (low priority)
 * @note No semaphore synchronization - reads gLive directly
 * @note Self-destructs if initialization fails
 */
void tiny_ml_task(void *pvParameters)
{
    // One-time initialization of TensorFlow Lite Micro
    setupTinyML();

    // Verify initialization succeeded
    // If input/output tensors are null, setup failed
    if (input == nullptr || output == nullptr)
    {
        Serial.println("[TinyML] Initialization failed, deleting task");
        vTaskDelete(nullptr);  // Self-destruct (remove task from scheduler)
        return;
    }

    // Main inference loop
    while (1)
    {
        // Step 1: Get latest sensor readings from global state
        // These are updated by Task 1 (DHT20 sensor) every 500ms
        float temperature = gLive.tC;
        float humidity    = gLive.rh;

        // Step 2: Wait for valid data (NAN = sensor not ready)
        if (isnan(temperature) || isnan(humidity))
        {
            // No valid data yet - wait and try again
            vTaskDelay(pdMS_TO_TICKS(TINYML_WAIT_FOR_DATA_MS));
            continue;
        }

        // Step 3: Copy sensor data into input tensor
        // Input tensor expects 2 floats: [temperature, humidity]
        input->data.f[0] = temperature;  // First input: temperature (°C)
        input->data.f[1] = humidity;     // Second input: humidity (%)

        // Step 4: Run neural network inference
        // This executes the forward pass through all layers
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk)
        {
            // Inference failed (rare - usually memory corruption)
            error_reporter->Report("Invoke failed");
            vTaskDelay(pdMS_TO_TICKS(TINYML_RETRY_DELAY_MS));
            continue;
        }

        // Step 5: Extract output (anomaly score)
        // Output tensor contains single float: 0.0 (normal) to 1.0 (anomalous)
        float result = output->data.f[0];
        
        // Log result to serial console
        Serial.printf("[TinyML] Score %.3f (T=%.1f°C H=%.1f%%)\n", result, temperature, humidity);

        // Step 6: Store result in global state (for web API)
        gLive.tinyml_score = result;           // Anomaly score
        gLive.tinyml_last_ms = millis();       // Timestamp
        gLive.tinyml_runs++;                   // Execution counter

        // Step 7: Wait 5 seconds before next inference
        // Inference is computationally expensive (~200-500ms)
        // 5-second interval balances responsiveness and CPU usage
        vTaskDelay(pdMS_TO_TICKS(TINYML_INFERENCE_MS));
    }
}

/* ====== Task Creation ====== */

/**
 * @brief Create TinyML FreeRTOS task
 * @details Creates a task with:
 *          - Name: "TinyML"
 *          - Stack: 8KB (large for ML operations)
 *          - Priority: 1 (low - ML is not time-critical)
 *          - Core: tskNO_AFFINITY (auto-balance)
 * 
 * @note Called by createAllTasks() in tasks.cpp
 * @note Task starts running immediately after creation
 */
void createTinyMLTask()
{
    // Create TinyML task with large stack (8KB)
    // ML operations require significant stack space
    BaseType_t created = xTaskCreatePinnedToCore(
        tiny_ml_task,           // Task function
        "TinyML",               // Task name (for debugging)
        TASK_TINYML_STACK_SIZE, // Stack size: 8192 bytes
        nullptr,                // Task parameter (unused)
        TASK_TINYML_PRIORITY,   // Priority: 1 (low)
        nullptr,                // Task handle (not needed)
        APP_CPU_NUM             // CPU core: tskNO_AFFINITY (auto-balance)
    );
    
    // Verify task creation succeeded
    if (created != pdPASS)
    {
        Serial.println("[TinyML] Failed to create TinyML task");
    }
    else
    {
        Serial.println("[TinyML] Task created");
    }
}
