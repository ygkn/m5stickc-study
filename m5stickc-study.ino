#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include "./config.h"

#define USE_SERIAL Serial

float a_x = 0.0f;
float a_y = 0.0f;
float a_z = 0.0f;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }

  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setRotation(1);

  Serial.printf("[Wi-Fi] Connect to %s.\n", WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  if ((WiFi.status() != WL_CONNECTED)) {
    while ((WiFi.status() != WL_CONNECTED)) {
      Serial.println("[Wi-Fi] Connecting...");
      delay(100);
    }
    Serial.printf("[Wi-Fi] Connected! IP Addr: %s\n", WiFi.localIP());
  }

  M5.update();
  M5.IMU.getAccelData(&a_x, &a_y, &a_z);

  StaticJsonDocument<48> doc;

  doc["x"] = a_x;
  doc["y"] = a_y;
  doc["z"] = a_z;

  char buffer[255];

  serializeJson(doc, buffer, sizeof(buffer));

  Serial.printf("[JSON] %s\n", buffer);

  HTTPClient http;
  http.begin(API_ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  int status_code = http.PUT((uint8_t*)buffer, strlen(buffer));

  if (status_code == 200) {
    Serial.println("[HTTP] OK!");
  } else {
    Serial.printf("[HTTP] NG, status: %d", status_code);
  }

  delay(1000);
}
