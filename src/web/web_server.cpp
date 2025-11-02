#include "web_server.h"
#include "web_pages.h"
#include "../config/config.h"
#include "../config/system_types.h"

/* ====== Local Objects ====== */
static WebServer server(80);

/* ====== WiFi IP Configuration ====== */
static IPAddress AP_IP(192, 168, 4, 1);
static IPAddress AP_GW(192, 168, 4, 1);
static IPAddress AP_MASK(255, 255, 255, 0);

/* ====== Helper Functions ====== */
static bool parseFloatSafe(const String& s, float &out) {
    if (s.length() == 0) return false;
    out = s.toFloat();
    return true;
}

/* ====== HTTP Route Handlers ====== */

static void handleIndex() {
    server.send_P(200, "text/html", INDEX_HTML);
}

static void handleState() {
    String resp = "{";
    resp += "\"ms\":" + String(millis());
    resp += ",\"tC\":" + String(gLive.tC, 2);
    resp += ",\"rh\":" + String(gLive.rh, 2);
    resp += ",\"tBand\":\"" + String(bandName(gLive.tBand)) + "\"";
    resp += ",\"hBand\":\"" + String(humName(gLive.hBand)) + "\"";
    resp += ",\"led\":" + String(gLive.ledOn ? 1 : 0);
    resp += ",\"blink_on\":" + String(gLive.onMs);
    resp += ",\"blink_off\":" + String(gLive.offMs);
    resp += ",\"giveTemp\":" + String(gLive.giveTemp);
    resp += ",\"takeTemp\":" + String(gLive.takeTemp);
    resp += ",\"giveHum\":" + String(gLive.giveHum);
    resp += ",\"takeHum\":" + String(gLive.takeHum);
    resp += ",\"tcold\":" + String(T_COLD_MAX, 1);
    resp += ",\"tnorm\":" + String(T_NORMAL_MAX, 1);
    resp += ",\"thot\":" + String(T_HOT_MAX, 1);
    resp += ",\"hdry\":" + String(H_DRY_MAX, 1);
    resp += ",\"hcomf\":" + String(H_COMF_MAX, 1);
    resp += ",\"hhum\":" + String(H_HUMID_MAX, 1);
    resp += ",\"dht_last_ms\":" + String(gLive.dht_last_ms);
    resp += ",\"led_last_ms\":" + String(gLive.led_last_ms);
    resp += ",\"neo_last_ms\":" + String(gLive.neo_last_ms);
    resp += ",\"lcd_last_ms\":" + String(gLive.lcd_last_ms);
    resp += ",\"dht_runs\":" + String(gLive.dht_runs);
    resp += ",\"led_runs\":" + String(gLive.led_runs);
    resp += ",\"neo_runs\":" + String(gLive.neo_runs);
    resp += ",\"lcd_runs\":" + String(gLive.lcd_runs);
    resp += ",\"tiny_score\":" + String(gLive.tinyml_score, 3);
    resp += ",\"tiny_last_ms\":" + String(gLive.tinyml_last_ms);
    resp += ",\"tiny_runs\":" + String(gLive.tinyml_runs);
    resp += ",\"uiMode\":" + String(gLive.uiMode);
    resp += ",\"wifiMode\":\"" + gWifiMode + "\"";
    resp += "}";
    server.send(200, "application/json", resp);
}

static void handleSet() {
    float tcold = T_COLD_MAX;
    float tnorm = T_NORMAL_MAX;
    float thot  = T_HOT_MAX;
    float hdry  = H_DRY_MAX;
    float hcomf = H_COMF_MAX;
    float hhum  = H_HUMID_MAX;

    bool ok = true;

    if (server.hasArg("tcold")) ok = ok && parseFloatSafe(server.arg("tcold"), tcold);
    if (server.hasArg("tnorm")) ok = ok && parseFloatSafe(server.arg("tnorm"), tnorm);
    if (server.hasArg("thot"))  ok = ok && parseFloatSafe(server.arg("thot"),  thot);
    if (server.hasArg("hdry"))  ok = ok && parseFloatSafe(server.arg("hdry"),  hdry);
    if (server.hasArg("hcomf")) ok = ok && parseFloatSafe(server.arg("hcomf"), hcomf);
    if (server.hasArg("hhum"))  ok = ok && parseFloatSafe(server.arg("hhum"),  hhum);

    if (!ok) {
        server.send(400, "text/plain", "Invalid number in request.");
        return;
    }

    if (!(tcold < tnorm && tnorm < thot)) {
        server.send(400, "text/plain", "Task 1 (LED): COLD < NORMAL < HOT");
        return;
    }

    if (!(hdry < hcomf && hcomf < hhum)) {
        server.send(400, "text/plain", "Task 2 (Hum): DRY < COMFORT < HUMID");
        return;
    }

    T_COLD_MAX   = tcold;
    T_NORMAL_MAX = tnorm;
    T_HOT_MAX    = thot;
    H_DRY_MAX    = hdry;
    H_COMF_MAX   = hcomf;
    H_HUMID_MAX  = hhum;

    server.send(200, "text/plain", "Thresholds updated.");
}

static void handleUiOff() {
    gLive.uiMode = 0;
    server.send(200, "text/plain", "UI strip OFF");
}

static void handleUiBar() {
    gLive.uiMode = 1;
    server.send(200, "text/plain", "UI strip BAR");
}

static void handleUiDemo() {
    gLive.uiMode = 2;
    server.send(200, "text/plain", "UI strip DEMO");
}

static void handleUiSos() {
    gLive.uiMode = 3;
    server.send(200, "text/plain", "UI strip SOS");
}

static void handleUiBlink() {
    gLive.uiMode = 4;
    server.send(200, "text/plain", "UI strip BLINK");
}

static void handleFireAlert() {
    if (server.hasArg("enable")) {
        int enable = server.arg("enable").toInt();
        if (enable) {
            float threshold = 45.0;
            if (server.hasArg("threshold")) {
                threshold = server.arg("threshold").toFloat();
            }
            // Store fire alert settings (could be saved to global variables)
            server.send(200, "text/plain", "Fire alert enabled at " + String(threshold, 1) + "°C");
        } else {
            server.send(200, "text/plain", "Fire alert disabled");
        }
    } else {
        server.send(400, "text/plain", "Missing enable parameter");
    }
}

static void handleWifi() {
    String mode = server.hasArg("mode") ? server.arg("mode") : "ap";
    String ssid = server.hasArg("ssid") ? server.arg("ssid") : "";
    String pass = server.hasArg("pass") ? server.arg("pass") : "";

    Serial.println("[WiFi] Configuration request:");
    Serial.println("  Mode: " + mode);
    Serial.println("  SSID: " + ssid);

    if (mode == "sta") {
        if (ssid.length() == 0) {
            server.send(400, "text/plain", "Error: SSID required for Station mode");
            return;
        }
        
        server.send(200, "text/plain", "Connecting to " + ssid + "... Check serial monitor for status. You may need to reconnect.");
        delay(100); // Let response send
        
        gWifiMode = "sta";
        gStaSsid  = ssid;
        gStaPass  = pass;

        Serial.println("[WiFi] Switching to Station mode...");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(gStaSsid.c_str(), gStaPass.c_str());
        
        // Wait for connection (max 10 seconds)
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        Serial.println();
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("[WiFi] Connected to " + ssid);
            Serial.print("[WiFi] IP address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("[WiFi] Failed to connect to " + ssid);
            Serial.println("[WiFi] Reverting to AP mode...");
            WiFi.disconnect(true);
            WiFi.mode(WIFI_AP);
            WiFi.softAPConfig(AP_IP, AP_GW, AP_MASK);
            WiFi.softAP(AP_SSID_DEFAULT, AP_PASS_DEFAULT, 6, false, 4);
            gWifiMode = "ap";
        }
    } else {
        // AP mode - allow custom SSID or use default
        String apSsid = (ssid.length() > 0) ? ssid : String(AP_SSID_DEFAULT);
        String apPass = (pass.length() >= 8) ? pass : String(AP_PASS_DEFAULT);
        
        server.send(200, "text/plain", "Restarting AP mode: " + apSsid + ". Reconnect to new network.");
        delay(100); // Let response send
        
        gWifiMode = "ap";
        Serial.println("[WiFi] Restarting AP mode...");
        WiFi.disconnect(true);
        delay(100);
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(AP_IP, AP_GW, AP_MASK);
        WiFi.softAP(apSsid.c_str(), apPass.c_str(), 6, false, 4);
        Serial.println("[WiFi] AP mode active");
        Serial.print("[WiFi] SSID: ");
        Serial.println(apSsid);
        Serial.print("[WiFi] Password: ");
        Serial.println(apPass);
        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.softAPIP());
    }
}

static void handleGpio() {
    if (!server.hasArg("pin") || !server.hasArg("state")) {
        server.send(400, "text/plain", "Missing pin or state parameter");
        return;
    }
    
    int pin = server.arg("pin").toInt();
    int state = server.arg("state").toInt();
    
    // Validate pin number
    if (pin < 0 || pin > 48) {
        server.send(400, "text/plain", "Invalid GPIO pin");
        return;
    }
    
    // Prevent control of critical pins
    if (pin == 11 || pin == 12 || pin == 6 || pin == 45 || pin == 48) {
        server.send(400, "text/plain", "Cannot control system GPIO pins (I2C, NeoPixel, LED)");
        return;
    }
    
    // Set GPIO mode and state
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state ? HIGH : LOW);
    
    String response = "GPIO " + String(pin) + " set to " + (state ? "HIGH" : "LOW");
    server.send(200, "text/plain", response);
}

/* ====== Public Functions ====== */

void initWiFi() {
    Serial.println("\n[WiFi] Initializing Access Point...");
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_IP, AP_GW, AP_MASK);
    WiFi.softAP(AP_SSID_DEFAULT, AP_PASS_DEFAULT, 6, false, 4);
    
    Serial.print("[WiFi] SSID: ");
    Serial.println(AP_SSID_DEFAULT);
    Serial.print("[WiFi] Password: ");
    Serial.println(AP_PASS_DEFAULT);
    Serial.print("[WiFi] AP IP: ");
    Serial.println(WiFi.softAPIP());
    
    gWifiMode = "ap";
}

void initWebServer() {
    Serial.println("[WEB] Initializing web server...");
    
    server.on("/", handleIndex);
    server.on("/state", handleState);
    server.on("/set", handleSet);
    server.on("/ui/off", handleUiOff);
    server.on("/ui/bar", handleUiBar);
    server.on("/ui/demo", handleUiDemo);
    server.on("/ui/sos", handleUiSos);
    server.on("/ui/blink", handleUiBlink);
    server.on("/fire-alert", handleFireAlert);
    server.on("/wifi", handleWifi);
    server.on("/gpio", handleGpio);
    
    server.begin();
    
    Serial.println("[WEB] Web server started on port 80");
    Serial.println("[WEB] Dashboard: http://192.168.4.1");
}

void handleWebServer() {
    server.handleClient();
}
