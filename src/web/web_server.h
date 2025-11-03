/**
 * @file web_server.h
 * @brief Web Server Interface - HTTP server and WiFi management
 * @author ESP32-S3 Lab
 * @date 2025
 * 
 * This header declares the public interface for the web server subsystem.
 * The web server provides:
 * - Real-time system monitoring via JSON API
 * - Interactive HTML dashboard for configuration
 * - WiFi Access Point and Station mode support
 * - Remote GPIO control
 * - Threshold configuration
 * - NeoPixel UI mode control
 * 
 * Default Access:
 * - SSID: ESP32-S3-LAB
 * - Password: 12345678
 * - URL: http://192.168.4.1
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

/* ====== Web Server Functions ====== */

/**
 * @brief Initialize WiFi in Access Point mode
 * @details Creates a WiFi network with:
 *          - SSID: ESP32-S3-LAB (configurable)
 *          - Password: 12345678 (configurable)
 *          - IP: 192.168.4.1 (fixed)
 *          - Gateway: 192.168.4.1
 *          - Subnet: 255.255.255.0
 *          - Max clients: 4
 * @note Call this once during system setup
 * @note Can be switched to Station mode via web API
 */
void initWiFi();

/**
 * @brief Initialize web server and register HTTP routes
 * @details Registers the following endpoints:
 *          - GET  /          : HTML dashboard
 *          - GET  /state     : JSON system state
 *          - POST /set       : Update thresholds
 *          - POST /ui/*      : Control NeoPixel modes
 *          - POST /fire-alert: Fire alert control
 *          - POST /wifi      : WiFi configuration
 *          - POST /gpio      : GPIO control
 * @note Call this after initWiFi() and before starting main loop
 * @note Server runs on port 80 (HTTP)
 */
void initWebServer();

/**
 * @brief Process incoming HTTP client requests
 * @details This function must be called repeatedly in the main loop
 *          to handle incoming web requests. It processes one request
 *          per call and returns immediately if no request is pending.
 * @note Non-blocking function - safe to call in tight loop
 * @note Typical usage: Call every 2-10ms in loop()
 * @warning Do not call from FreeRTOS tasks (not thread-safe)
 */
void handleWebServer();

#endif // WEB_SERVER_H
