#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

/* ====== Web Server Functions ====== */

/**
 * @brief Initialize WiFi in Access Point mode
 */
void initWiFi();

/**
 * @brief Initialize web server routes
 */
void initWebServer();

/**
 * @brief Handle client requests (call in main loop)
 */
void handleWebServer();

#endif // WEB_SERVER_H
