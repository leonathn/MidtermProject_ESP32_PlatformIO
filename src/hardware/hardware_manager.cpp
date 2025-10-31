#include "hardware_manager.h"

/* ====== Hardware Object Definitions ====== */
DHT20 dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_NeoPixel stripHum(NEOPIXEL_HUM_NUM, NEOPIXEL_HUM_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripUI(NEOPIXEL_UI_NUM, NEOPIXEL_UI_PIN, NEO_GRB + NEO_KHZ800);

/* ====== Semaphore Definitions ====== */
SemaphoreHandle_t semBandChanged = NULL;
SemaphoreHandle_t semHumChanged = NULL;
SemaphoreHandle_t semLcdUpdate = NULL;

/* ====== Initialization Functions ====== */
void initHardware() {
    // Initialize I2C for DHT20 and LCD
    Wire.begin(SDA_PIN, SCL_PIN);
    
    // Initialize DHT20 sensor
    dht.begin();
    
    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESP32-S3 LAB");
    
    // Initialize NeoPixel strips
    stripHum.begin();
    stripHum.show();
    
    stripUI.begin();
    stripUI.show();
    
    Serial.println("[HW] Hardware initialized successfully");
}

void initSemaphores() {
    semBandChanged = xSemaphoreCreateBinary();
    semHumChanged  = xSemaphoreCreateBinary();
    semLcdUpdate   = xSemaphoreCreateBinary();
    
    if (semBandChanged == NULL || semHumChanged == NULL || semLcdUpdate == NULL) {
        Serial.println("[ERROR] Failed to create semaphores!");
    } else {
        Serial.println("[SYNC] Semaphores created successfully");
    }
}
