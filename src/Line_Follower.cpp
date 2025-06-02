#include <Arduino.h>

// --- Konfigurasi Pin ---
// Sensor Api
const int FLAME_SENSOR_PIN = 11; // Pin sensor api (Digital)

// Kipas Pemadam Api
const int FAN_PIN = A0; // Pin untuk mengontrol kipas (misalnya melalui relay atau transistor)

// Sensor Garis (Digital)
// Asumsi: LOW = Mendeteksi Garis Hitam, HIGH = Mendeteksi Permukaan Putih
const int LINE_SENSOR_LEFTMOST_PIN = 2;   // Sensor garis paling kiri
const int LINE_SENSOR_LEFT_PIN = 3;       // Sensor garis kiri
const int LINE_SENSOR_CENTER_PIN = 4;     // Sensor garis tengah
const int LINE_SENSOR_RIGHT_PIN = 5;      // Sensor garis kanan
const int LINE_SENSOR_RIGHTMOST_PIN = 6;  // Sensor garis paling kanan

// Driver Motor (misal L298N)
// Motor A (Kiri)
const int ENA_PIN = 9;   // Pin Enable A (PWM untuk kecepatan motor kiri)
const int IN1_PIN = 2;   // Pin Input 1 Motor A
const int IN2_PIN = 3;   // Pin Input 2 Motor A
// Motor B (Kanan)
const int ENB_PIN = 10;  // Pin Enable B (PWM untuk kecepatan motor kanan)
const int IN3_PIN = 4;  // Pin Input 3 Motor B
const int IN4_PIN = 5;  // Pin Input 4 Motor B

// --- Variabel PID ---
float Kp = 20.0; // Proportional gain ( perlu dituning )
float Ki = 0.5;  // Integral gain ( perlu dituning )
float Kd = 15.0; // Derivative gain ( perlu dituning )

float error = 0;
float previous_error = 0;
float integral = 0;
float derivative = 0;
float pid_output = 0;

// Batas untuk integral (Anti-windup)
float integral_limit = 100.0;

// --- Pengaturan Kecepatan Motor ---
int baseSpeed = 100; // Kecepatan dasar motor (0-255) ( perlu dituning )
int turnSpeed = 80;  // Kecepatan saat berbelok

// --- Setup ---
void setup() {
  Serial.begin(9600);

  // Inisialisasi Pin Sensor Api
  pinMode(FLAME_SENSOR_PIN, INPUT);

  // Inisialisasi Pin Kipas
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW); // Pastikan kipas mati saat awal

  // Inisialisasi Pin Sensor Garis
  pinMode(LINE_SENSOR_LEFTMOST_PIN, INPUT);
  pinMode(LINE_SENSOR_LEFT_PIN, INPUT);
  pinMode(LINE_SENSOR_CENTER_PIN, INPUT);
  pinMode(LINE_SENSOR_RIGHT_PIN, INPUT);
  pinMode(LINE_SENSOR_RIGHTMOST_PIN, INPUT);

  // Inisialisasi Pin Motor
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  stopMotors(); // Pastikan motor berhenti saat awal
  Serial.println("Robot siap.");
}

// --- Loop Utama ---
void loop() {
  // 1. Deteksi Api
  // Asumsi sensor api HIGH saat ada api. Sesuaikan jika perlu.
  bool fireDetected = (digitalRead(FLAME_SENSOR_PIN) == HIGH);

  if (fireDetected) {
    Serial.println("Api terdeteksi!");
    stopMotors();
    digitalWrite(FAN_PIN, HIGH); // Nyalakan kipas

    // Tetap dalam mode pemadaman selama api masih terdeteksi
    while (digitalRead(FLAME_SENSOR_PIN) == HIGH) {
      Serial.println("Memadamkan api...");
      delay(100); // Jeda singkat
    }
    digitalWrite(FAN_PIN, LOW); // Matikan kipas jika api sudah padam
    Serial.println("Api padam, kipas dimatikan.");
    delay(2000); // Jeda sebelum melanjutkan
    resetPID(); // Reset PID setelah pemadaman
  } else {
    digitalWrite(FAN_PIN, LOW); // Pastikan kipas mati jika tidak ada api

    // 2. Baca Sensor Garis (5 sensor)
    // Asumsi sensor LOW saat di atas garis hitam
    bool sLM = (digitalRead(LINE_SENSOR_LEFTMOST_PIN) == LOW);
    bool sL  = (digitalRead(LINE_SENSOR_LEFT_PIN) == LOW);
    bool sC  = (digitalRead(LINE_SENSOR_CENTER_PIN) == LOW);
    bool sR  = (digitalRead(LINE_SENSOR_RIGHT_PIN) == LOW);
    bool sRM = (digitalRead(LINE_SENSOR_RIGHTMOST_PIN) == LOW);

    // Penentuan error berdasarkan posisi garis
    // -2: sangat kiri, -1: kiri, 0: tengah, 1: kanan, 2: sangat kanan
    if (sC && !sL && !sR && !sLM && !sRM) { // 00100
      error = 0;
    } else if (sL && !sC && !sR && !sLM && !sRM) { // 01000
      error = -1;
    } else if (sR && !sC && !sL && !sLM && !sRM) { // 00010
      error = 1;
    } else if (sLM && !sL && !sC && !sR && !sRM) { // 10000
      error = -2;
    } else if (sRM && !sR && !sC && !sL && !sLM) { // 00001
      error = 2;
    } else if (sL && sC && !sR && !sLM && !sRM) { // 01100
      error = -0.5;
    } else if (sR && sC && !sL && !sLM && !sRM) { // 00110
      error = 0.5;
    } else if (sLM && sL && !sC && !sR && !sRM) { // 11000
      error = -1.5;
    } else if (sRM && sR && !sC && !sL && !sLM) { // 00011
      error = 1.5;
    } else if (sL && sC && sR) { // 01110
      error = 0;
    } else if (sLM && sL && sC) { // 11100
      error = -1;
    } else if (sRM && sR && sC) { // 00111
      error = 1;
    } else if (sLM && sL && sC && sR && sRM) { // 11111 (persimpangan)
      error = 0;
    } else {
      // Tidak ada garis terdeteksi
      Serial.println("Tidak ada garis. Diam.");
      stopMotors();
      resetPID();
      delay(10);
      return;
    }

    // Kalkulasi PID
    integral += error;
    // Batasi integral (anti-windup)
    integral = constrain(integral, -integral_limit, integral_limit);

    derivative = error - previous_error;
    previous_error = error;

    pid_output = (Kp * error) + (Ki * integral) + (Kd * derivative);

    // Atur kecepatan motor berdasarkan output PID
    int leftMotorSpeed = baseSpeed + (int)pid_output;
    int rightMotorSpeed = baseSpeed - (int)pid_output;

    // Batasi kecepatan motor antara 0 dan 255
    leftMotorSpeed = constrain(leftMotorSpeed, 0, 255);
    rightMotorSpeed = constrain(rightMotorSpeed, 0, 255);

    // Logika belok tajam jika error sangat besar
    if (error <= -1.5) {
      turnLeftSharp(turnSpeed);
      resetPID();
    } else if (error >= 1.5) {
      turnRightSharp(turnSpeed);
      resetPID();
    } else if (!sLM && !sL && !sC && !sR && !sRM) {
      // Tidak ada garis terdeteksi, motor berhenti
      stopMotors();
      resetPID();
    } else {
      moveForward(leftMotorSpeed, rightMotorSpeed);
    }
  }
  delay(10); // Jeda loop kecil untuk stabilitas
}

// --- Fungsi Kontrol Motor ---
void moveForward(int speedLeft, int speedRight) {
  // Motor Kiri Maju
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, speedLeft);

  // Motor Kanan Maju
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENB_PIN, speedRight);
}

void turnLeftSharp(int speed) {
  // Motor Kiri Mundur (atau berhenti untuk belok lebih halus)
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  analogWrite(ENA_PIN, speed);

  // Motor Kanan Maju
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENB_PIN, speed);
}

void turnRightSharp(int speed) {
  // Motor Kiri Maju
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, speed);

  // Motor Kanan Mundur (atau berhenti untuk belok lebih halus)
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENB_PIN, speed);
}

void stopMotors() {
  // Matikan Motor Kiri
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, 0);

  // Matikan Motor Kanan
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENB_PIN, 0);
}

// --- Fungsi Bantu ---
void resetPID() {
  error = 0;
  previous_error = 0;
  integral = 0;
  derivative = 0;
  pid_output = 0;
}