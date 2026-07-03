/*
  DIAGNOSTIC SKETCH - run this BEFORE going back to the main project
  Tests: 1) I2C scan (finds OLED address)  2) Ultrasonic live readings
*/

#include <Wire.h>

#define TRIG_PIN 5
#define ECHO_PIN 18

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(21, 22);   // SDA, SCL

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("\n--- I2C Scanner ---");
  byte count = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found I2C device at 0x");
      Serial.println(addr, HEX);
      count++;
    }
  }
  if (count == 0) {
    Serial.println("No I2C device found! Check OLED wiring/power.");
  }
  Serial.println("--- Starting ultrasonic test (move your hand near sensor) ---");
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    Serial.println("No echo received - check ECHO wiring!");
  } else {
    float distance = duration * 0.0343 / 2.0;
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  delay(500);
}
