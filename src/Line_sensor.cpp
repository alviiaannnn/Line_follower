// Array untuk menyimpan nilai analog dari 8 sensor
#include <HardwareSerial.h>
#include <pins_arduino.h>
#include <Arduino.h>

int sensorPins[8] = {A0, A1, A2, A3, A4, A5};
int clpPins = 12;
int sensorValues[8];

void setup() {
  Serial.begin(9600);  // Mulai Serial Monitor
}

void loop() {
  // Baca semua nilai sensor
  for (int i = 0; i < 8; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
  }

  // Tampilkan nilai ke Serial Monitor
  for (int i = 0; i < 8; i++) {
    Serial.print("A");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(sensorValues[i]);
    Serial.print(" CLP : ");
    Serial.print(clpPins);
    Serial.print("\t");
  }
  Serial.println(); // baris baru

  delay(100); // jeda pembacaan
}
