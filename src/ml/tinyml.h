/**
 * @file tinyml.h
 * @brief TinyML Interface - TensorFlow Lite Micro for ESP32
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * This header provides the interface for running TensorFlow Lite Micro
 * neural network inference on ESP32-S3 for anomaly detection.
 * 
 * Purpose:
 * ========
 * Detects unusual patterns in temperature and humidity readings using
 * a trained neural network model. The model runs directly on the ESP32-S3
 * without requiring cloud connectivity or external processing.
 * 
 * TensorFlow Lite Micro:
 * ======================
 * TensorFlow Lite Micro (TFLM) is a lightweight machine learning framework
 * designed for microcontrollers. It enables on-device inference with:
 * - Minimal memory footprint (8KB tensor arena)
 * - No dynamic memory allocation
 * - Integer and floating-point operations
 * - Support for common neural network layers
 * 
 * Model Architecture:
 * ===================
 * Input:  2 float values [temperature, humidity]
 * Hidden: 1-3 layers (Dense/ReLU/etc.) - model dependent
 * Output: 1 float value [anomaly_score] (0.0 = normal, 1.0 = anomalous)
 * 
 * The model is trained offline using Python/TensorFlow and converted to
 * TensorFlow Lite format (.tflite), then embedded as C array in
 * dht_anomaly_model.h for compilation into firmware.
 * 
 * Usage Flow:
 * ===========
 * 1. setupTinyML() - Initialize interpreter (one-time)
 * 2. Copy sensor data to input tensor
 * 3. interpreter->Invoke() - Run inference
 * 4. Read anomaly score from output tensor
 * 5. Update gLive.tinyml_score for web API
 * 
 * Performance:
 * ============
 * - Inference time: 200-500ms (depends on model complexity)
 * - Memory usage: 8KB tensor arena + model size (~10-50KB)
 * - Power impact: Minimal (runs at low priority, 5s interval)
 * 
 * Training Your Own Model:
 * ========================
 * 1. Collect training data (normal and anomalous readings)
 * 2. Train model in Python using TensorFlow/Keras
 * 3. Convert to TensorFlow Lite: converter.convert()
 * 4. Generate C array: xxd -i model.tflite > model.h
 * 5. Replace dht_anomaly_model.h with your model
 * 6. Recompile and upload firmware
 * 
 * Resources:
 * ==========
 * - TensorFlow Lite Micro: https://www.tensorflow.org/lite/microcontrollers
 * - ESP32 Guide: https://github.com/espressif/esp-tflite-micro
 * - Training Tutorial: https://www.edgeimpulse.com/
 */

#ifndef __TINY_ML__
#define __TINY_ML__

#include <Arduino.h>

// Model definition (trained neural network as C array)
#include "../ml/dht_anomaly_model.h"

// System configuration and state
#include "../config/config.h"
#include "../config/system_types.h"

// TensorFlow Lite Micro core library
#include <TensorFlowLite_ESP32.h>

// TensorFlow Lite Micro components
#include "tensorflow/lite/micro/all_ops_resolver.h"        // Operation registry (Dense, ReLU, etc.)
#include "tensorflow/lite/micro/micro_error_reporter.h"    // Error logging to serial
#include "tensorflow/lite/micro/micro_interpreter.h"       // Neural network executor
#include "tensorflow/lite/micro/system_setup.h"            // Platform initialization
#include "tensorflow/lite/schema/schema_generated.h"       // Model schema definitions

/* ====== Function Prototypes ====== */

/**
 * @brief Initialize TensorFlow Lite Micro interpreter
 * @details One-time setup that:
 *          - Loads neural network model from flash
 *          - Creates operation resolver
 *          - Allocates tensor memory (8KB arena)
 *          - Validates model compatibility
 * @note Called once when TinyML task starts
 * @warning Must be called before running inference
 */
void setupTinyML();

/**
 * @brief FreeRTOS task for continuous ML inference
 * @param pvParameters Unused (required by FreeRTOS API)
 * @details Infinite loop that:
 *          - Reads sensor data from gLive
 *          - Runs neural network inference
 *          - Stores anomaly score in gLive.tinyml_score
 *          - Repeats every 5 seconds
 * @note Runs at Priority 1 (low priority)
 * @note Self-destructs if initialization fails
 */
void tiny_ml_task(void *pvParameters);

/**
 * @brief Create TinyML FreeRTOS task
 * @details Wrapper function that creates the TinyML task with:
 *          - Stack: 8192 bytes (large for ML operations)
 *          - Priority: 1 (low - not time-critical)
 *          - Core: tskNO_AFFINITY (auto load-balance)
 * @note Called by createAllTasks() during system initialization
 * @note Task starts running immediately after creation
 */
void createTinyMLTask();

#endif // __TINY_ML__
