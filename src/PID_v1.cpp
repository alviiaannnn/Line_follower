#include <Arduino.h>

// --- Konfigurasi Pin ---
const int LINE_SENSOR_LEFTMOST_PIN = A5;
const int LINE_SENSOR_LEFT_PIN     = A4;
const int LINE_SENSOR_CENTER_PIN   = A3;
const int LINE_SENSOR_RIGHT_PIN    = A2;
const int LINE_SENSOR_RIGHTMOST_PIN= A1;

// Motor A (Kiri)
const int ENA_PIN = 9;
const int IN1_PIN = 2;
const int IN2_PIN = 3;
// Motor B (Kanan)
const int ENB_PIN = 10;
const int IN3_PIN = 4;
const int IN4_PIN = 5;

// Kecepatan motor
int baseSpeed = 90;
int turnSpeed = 90;

void setup() {
  pinMode(LINE_SENSOR_LEFTMOST_PIN, INPUT);
  pinMode(LINE_SENSOR_LEFT_PIN, INPUT);
  pinMode(LINE_SENSOR_CENTER_PIN, INPUT);
  pinMode(LINE_SENSOR_RIGHT_PIN, INPUT);
  pinMode(LINE_SENSOR_RIGHTMOST_PIN, INPUT);

  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  // Matikan motor di awal
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, 0);
}

void loop() {
  bool sLM = (digitalRead(LINE_SENSOR_LEFTMOST_PIN) == LOW);
  bool sL  = (digitalRead(LINE_SENSOR_LEFT_PIN) == LOW);
  bool sC  = (digitalRead(LINE_SENSOR_CENTER_PIN) == LOW);
  bool sR  = (digitalRead(LINE_SENSOR_RIGHT_PIN) == LOW);
  bool sRM = (digitalRead(LINE_SENSOR_RIGHTMOST_PIN) == LOW);

  if (sC && !sL && !sR) {
    // Maju lurus
    moveForward(baseSpeed, baseSpeed);
  } else if (sL) {
    // Belok kiri
    moveForward(turnSpeed, baseSpeed);
  } else if (sR) {
    // Belok kanan
    moveForward(baseSpeed, turnSpeed);
  } else if (sLM) {
    // Belok kiri tajam
    moveForward(0, baseSpeed);
  } else if (sRM) {
    // Belok kanan tajam
    moveForward(baseSpeed, 0);
  } else {
    // Tidak ada garis, lakukan pencarian garis
    findLine();
  }

  delay(10);
}

void moveForward(int speedLeft, int speedRight) {
  // Motor kiri maju
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, speedLeft);

  // Motor kanan maju
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENB_PIN, speedRight);
}

void stopMotors() {
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, 0);
}

// Fungsi untuk mencari garis jika robot kehilangan garis
void findLine() {
  // Berhenti sejenak
  stopMotors();
  delay(100);

  // Coba putar ke kiri perlahan untuk mencari garis
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  analogWrite(ENA_PIN, baseSpeed / 2);

  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENB_PIN, baseSpeed / 2);

  delay(200);

  // Berhenti setelah mencoba mencari garis
  stopMotors();
}