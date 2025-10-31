#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/* ====== Hardware Pins ====== */
#ifndef LED_GPIO
  #define LED_GPIO GPIO_NUM_48
#endif

static const int SDA_PIN = 11;
static const int SCL_PIN = 12;

#define NEOPIXEL_HUM_PIN 45
#define NEOPIXEL_HUM_NUM 1

#define NEOPIXEL_UI_PIN 6
#define NEOPIXEL_UI_NUM 4

#ifndef APP_CPU_NUM
  #define APP_CPU_NUM tskNO_AFFINITY
#endif

/* ====== WiFi Configuration ====== */
#define AP_SSID_DEFAULT "ESP32-S3-LAB"
#define AP_PASS_DEFAULT "12345678"

/* ====== Default Thresholds ====== */
#define DEFAULT_T_COLD_MAX   20.0f
#define DEFAULT_T_NORMAL_MAX 30.0f
#define DEFAULT_T_HOT_MAX    40.0f

#define DEFAULT_H_DRY_MAX    40.0f
#define DEFAULT_H_COMF_MAX   60.0f
#define DEFAULT_H_HUMID_MAX  80.0f

/* ====== Task Configuration ====== */
#define TASK_DHT_STACK_SIZE     4096
#define TASK_LED_STACK_SIZE     3072
#define TASK_NEO_HUM_STACK_SIZE 3072
#define TASK_NEO_UI_STACK_SIZE  3072
#define TASK_LCD_STACK_SIZE     3072

#define TASK_DHT_PRIORITY       3
#define TASK_LED_PRIORITY       2
#define TASK_NEO_PRIORITY       2
#define TASK_LCD_PRIORITY       1

/* ====== Timing ====== */
#define DHT_READ_INTERVAL_MS    500
#define UI_STRIP_UPDATE_MS      120
#define WEB_POLL_INTERVAL_MS    500

#endif // CONFIG_H
