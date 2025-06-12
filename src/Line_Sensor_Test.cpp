#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    for (int pin = A1; pin <= A5; pin++) {
        pinMode(pin, INPUT);
    }
}

void loop() {
    Serial.print("Sensor values: ");
    for (int pin = A1; pin <= A5; pin++) {
        int value = analogRead(pin);
        Serial.print("A");
        Serial.print(pin - A0);
        Serial.print(": ");
        Serial.print(value);
        if (pin < A5) Serial.print(", ");
    }
    Serial.println();
    delay(500);
}