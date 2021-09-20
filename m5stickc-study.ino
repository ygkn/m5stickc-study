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
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);

  double vbat = M5.Axp.GetVbatData() * 1.1 / 1000;
  int bat_charge_p = int8_t((vbat - 3.0) / 1.2 * 100);
  if (bat_charge_p > 100) {
    bat_charge_p = 100;
  } else if (bat_charge_p < 0) {
    bat_charge_p = 0;
  }

  M5.Lcd.printf("CHARGE: %3d%\n\n", bat_charge_p);

  if ((WiFi.status() != WL_CONNECTED)) {
    M5.Lcd.printf("Connecting to %s...\n", WIFI_SSID);
  } else {
    M5.Lcd.printf("Connected to %s, IP: %s\n", WIFI_SSID, WiFi.localIP());
  }

  M5.update();
  M5.IMU.getAccelData(&a_x, &a_y, &a_z);

  M5.Lcd.printf("\nx: %2.3f\ny: %2.3f\nz: %2.3f\n\n", a_x, a_y, a_z);

  if ((WiFi.status() == WL_CONNECTED)) {
    StaticJsonDocument<48> doc;

    doc["x"] = a_x;
    doc["y"] = a_y;
    doc["z"] = a_z;

    char buffer[255];

    serializeJson(doc, buffer, sizeof(buffer));

    M5.Lcd.printf("Sending... ");
    Serial.printf("[JSON] %s\n", buffer);

    HTTPClient http;
    http.begin(API_ENDPOINT);
    http.addHeader("Content-Type", "application/json");
    int status_code = http.PUT((uint8_t*)buffer, strlen(buffer));

    if (status_code == 200) {
      Serial.println("[HTTP] OK!");
      M5.Lcd.printf("OK!");
    } else {
      Serial.printf("[HTTP] NG, status: %d", status_code);
      M5.Lcd.printf("NG, status: %d", status_code);
    }
  }

  delay(1000);
}
