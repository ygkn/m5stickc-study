#include <M5StickC.h>

float a_x = 0.0f;
float a_y = 0.0f;
float a_z = 0.0f;

void setup() {
  Serial.begin(9600);
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setRotation(1);
}

void loop() {
  M5.update();
  M5.IMU.getAccelData(&a_x, &a_y, &a_z);

  Serial.printf("(%5.2f, %5.2f, %5.2f)\n", a_x, a_y, a_z);
  delay(1000);
}
