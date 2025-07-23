/**
 * @file F-ESP32-PICO-D4.ino
 * @author Osman Selçuk SELÇUKLU 
 * @brief IoT-Tea-Tracking-System
 * @version 1.0.0
 * @date 23 July 2025
 * 
 * @copyright OSS EMBEDDED SYSTEM Copyright (C) 2025. See: license/license.txt
 * @link --
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <esp_sleep.h>
#include <esp_wifi.h>
#include <esp_bt.h>

// ----- User Settings -----
const char* WIFI_SSID   = "YOUR_WIFI_SSID";
const char* WIFI_PASS   = "YOUR_WIFI_PASSWORD";
const char* MAIN_HOST   = "10.0.0.77";  //192.168.1.100
const uint16_t MAIN_PORT = 80;

// Floor ID (Change every card!)
#define FLOOR_ID 3

// Pin Definitions
#define BUTTON_PIN     25  // GPIO25, button connected to GND; goes HIGH when pressed
#define NEO_ENABLE_PIN  5  // GPIO5, NeoPixel active when LOW
#define NEO_PIN         4  // GPIO4, NeoPixel data
#define NEO_COUNT       1  //

// ULTRA LOW POWER: Longer heartbeat intervals
const unsigned long HB_INTERVAL_MS = 60000UL; // 60s heartbeat for 400mAh battery

// Wi‑Fi connection timeout - keep it short
const unsigned long WIFI_CONNECT_TIMEOUT_MS = 8000UL;

// Deep sleep - longer intervals for battery conservation
const unsigned long SLEEP_DURATION_US = 55000000UL; // 55s sleep (wake 5s before heartbeat)

// RTC Memory - minimal state preservation
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR bool serverConnected = false;
RTC_DATA_ATTR unsigned long lastHBTime = 0;

// Session variables
bool wifiConnected = false;

// NeoPixel
Adafruit_NeoPixel strip(NEO_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

// ----- Ultra Power Saving -----
void enableUltraPowerSaving() {
  // Disable Bluetooth
  esp_bt_controller_disable();
  esp_bt_mem_release(ESP_BT_MODE_BTDM);
  
  // Minimum stable CPU frequency
  setCpuFrequencyMhz(80);
  
  // Maximum WiFi power saving
  esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
}

void initNeoPixel() {
  pinMode(NEO_ENABLE_PIN, OUTPUT);
  digitalWrite(NEO_ENABLE_PIN, HIGH); 
  strip.begin();
  strip.setBrightness(0);
  strip.clear();
  strip.show();
}

void disableNeoPixel() {
  digitalWrite(NEO_ENABLE_PIN, HIGH);
  strip.clear();
  strip.show();
}

// ----- Minimal Network Functions -----
String makeURL(const String& path) {
  return String("http://") + MAIN_HOST + ":" + MAIN_PORT + path;
}

bool quickGET(const String& url, const char* tag) {
  WiFiClient client;
  client.setTimeout(2000); 
  HTTPClient http;
  
  if(!http.begin(client, url)) {
    Serial.printf("[%s] FAIL\n", tag);
    return false;
  }
  
  http.setTimeout(2000);
  int code = http.GET();
  http.end();
  client.stop();
  
  Serial.printf("[%s] -> %d\n", tag, code);
  return (code == HTTP_CODE_OK);
}

bool connectWiFiQuick() {
  if (WiFi.status() == WL_CONNECTED) return true;
  
  Serial.print("WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
    delay(200);
    Serial.print(".");
  }
  
  wifiConnected = (WiFi.status() == WL_CONNECTED);
  if (wifiConnected) {
    Serial.printf("OK IP:%s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("FAIL");
  }
  
  return wifiConnected;
}

void disconnectWiFi() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  wifiConnected = false;
}

void sendConnect() {
  if(!wifiConnected) return;
  
  String url = makeURL("/connect?floor=" + String(FLOOR_ID) 
                      + "&rssi=" + String(WiFi.RSSI())
                      + "&ip=" + WiFi.localIP().toString());
  
  bool success = quickGET(url, "CONN");
  serverConnected = success;
}

void sendHeartbeat() {
  if(!wifiConnected) return;
  
  if(!serverConnected) {
    sendConnect();
    return;
  }
  
  String url = makeURL("/heartbeat?floor=" + String(FLOOR_ID) 
                      + "&rssi=" + String(WiFi.RSSI()));
  
  quickGET(url, "HB");
  lastHBTime = millis();
}

void sendButtonPress() {
  if(!wifiConnected) return;
  
  String url = makeURL("/start?floor=" + String(FLOOR_ID) 
                      + "&rssi=" + String(WiFi.RSSI()));
  
  bool success = quickGET(url, "BTN");
  
  // Simple feedback - no complex fading to save power
  digitalWrite(NEO_ENABLE_PIN, LOW);
  if(success) {
    // Success: Brief green flash
    strip.setBrightness(150);
    strip.setPixelColor(0, strip.Color(0,255,0));
    strip.show();
    delay(150);
  } else {
    // Fail: Brief red flash  
    strip.setBrightness(150);
    strip.setPixelColor(0, strip.Color(255,0,0));
    strip.show();
    delay(150);
  }
  
  strip.clear();
  strip.show();
  digitalWrite(NEO_ENABLE_PIN, HIGH);
}

void quickButtonFeedback() {
  // Immediate visual feedback without network
  digitalWrite(NEO_ENABLE_PIN, LOW);
  strip.setBrightness(100);
  strip.setPixelColor(0, strip.Color(0,150,150)); // Turkuaz
  strip.show();
  delay(100);
  strip.clear();
  strip.show();
  digitalWrite(NEO_ENABLE_PIN, HIGH);
}

void enterDeepSleep() {
  Serial.println("Sleep...");
  
  // Button wake-up
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, HIGH);
  
  // Timer wake-up for heartbeat
  esp_sleep_enable_timer_wakeup(SLEEP_DURATION_US);
  
  // Power down everything
  disableNeoPixel();
  disconnectWiFi();
  
  delay(50);
  esp_deep_sleep_start();
}

// ----- setup -----
void setup(){
  Serial.begin(115200);
  delay(100);
  
  bootCount++;
  Serial.printf("\n=== BEACON Boot #%d ===\n", bootCount);
  
  // Ultra power saving mode
  enableUltraPowerSaving();
  
  // Hardware setup
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  initNeoPixel();
  
  // Check wake reason
  esp_sleep_wakeup_cause_t wakeup = esp_sleep_get_wakeup_cause();
  bool button_wake = (wakeup == ESP_SLEEP_WAKEUP_EXT0);
  bool timer_wake = (wakeup == ESP_SLEEP_WAKEUP_TIMER);
  bool first_boot = (bootCount == 1);
  
  Serial.printf("Wake: %s\n", 
    button_wake ? "BUTTON" : 
    timer_wake ? "TIMER" : "BOOT");
  
  // Network operations - keep minimal
  bool needNetwork = button_wake || timer_wake || first_boot;
  
  if (needNetwork) {
    if (connectWiFiQuick()) {
      
      if (button_wake) {
        Serial.println("Button press!");
        quickButtonFeedback();
        sendButtonPress();
      }
      
      if (timer_wake || first_boot) {
        sendHeartbeat();
      }
      
      // Keep network time minimal
      delay(500);
      disconnectWiFi();
    }
  }
  
  // Quick check for immediate button press
  delay(50);
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println("Immediate button!");
    quickButtonFeedback();
    if (connectWiFiQuick()) {
      sendButtonPress();
    }
  }
  
  // Go back to sleep immediately
  enterDeepSleep();
}

// ----- loop -----
void loop(){
  // Should never reach here due to deep sleep
  delay(1000);
  enterDeepSleep();
}