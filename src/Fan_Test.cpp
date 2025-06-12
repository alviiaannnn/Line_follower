#include <Arduino.h>

#define NEAR 7
#define FAN_PIN 11
#define FLAME_SENSOR_PIN 13

void setup() {
    Serial.begin(9600);
    pinMode(NEAR, INPUT);
    pinMode(FLAME_SENSOR_PIN, INPUT);
    pinMode(FAN_PIN, OUTPUT);
}

void loop() {
    int nearState = digitalRead(NEAR);
    int flameState = digitalRead(FLAME_SENSOR_PIN);

    // Cek status semua pin input
    Serial.print("NEAR: ");
    Serial.println(nearState == HIGH ? "HIGH" : "LOW");
    Serial.print("FLAME_SENSOR_PIN: ");
    Serial.println(flameState == HIGH ? "HIGH" : "LOW");

    // Metode 1: Menggunakan NEAR sebagai parameter
    if (nearState == HIGH) {
        digitalWrite(FAN_PIN, HIGH); // Nyalakan kipas
        Serial.println("FAN ON (by NEAR)");
    } else {
        digitalWrite(FAN_PIN, LOW); // Matikan kipas
        Serial.println("FAN OFF (by NEAR)");
    }

    delay(1000);

    // Metode 2: Menggunakan FLAME_SENSOR_PIN sebagai parameter
    if (flameState == HIGH) {
        digitalWrite(FAN_PIN, HIGH); // Nyalakan kipas
        Serial.println("FAN ON (by FLAME SENSOR)");
    } else {
        digitalWrite(FAN_PIN, LOW); // Matikan kipas
        Serial.println("FAN OFF (by FLAME SENSOR)");
    }

    delay(1000);
}