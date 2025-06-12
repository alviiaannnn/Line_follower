#include <Arduino.h>

// Motor A (Kiri)
#define ENA_PIN 9
#define IN1_PIN 2
#define IN2_PIN 3
// Motor B (Kanan)
#define ENB_PIN 10
#define IN3_PIN 4
#define IN4_PIN 5

void setup() {
    // Set all motor pins as output
    pinMode(ENA_PIN, OUTPUT);
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(ENB_PIN, OUTPUT);
    pinMode(IN3_PIN, OUTPUT);
    pinMode(IN4_PIN, OUTPUT);
}

void loop() {
    // Test Motor A (Kiri) Forward
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    analogWrite(ENA_PIN, 200);
    delay(1000);

    // Test Motor A (Kiri) Backward
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    analogWrite(ENA_PIN, 200);
    delay(1000);

    // Stop Motor A
    analogWrite(ENA_PIN, 0);
    delay(500);

    // Test Motor B (Kanan) Forward
    digitalWrite(IN3_PIN, HIGH);
    digitalWrite(IN4_PIN, LOW);
    analogWrite(ENB_PIN, 200);
    delay(1000);

    // Test Motor B (Kanan) Backward
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, HIGH);
    analogWrite(ENB_PIN, 200);
    delay(1000);

    // Stop Motor B
    analogWrite(ENB_PIN, 0);
    delay(500);
}