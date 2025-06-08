#include <Arduino.h>

// Motor A (Kiri)
#define ENA_PIN 9
#define IN1_PIN 2
#define IN2_PIN 3
// Motor B (Kanan)
#define ENB_PIN 10
#define IN3_PIN 4
#define IN4_PIN 5
// CLP & Kipas
#define CLP 12
#define FAN_PIN 11
#define FLAME_SENSOR_PIN 13

int maxSpeed = 200;
int baseSpeed = 150;

// PID variables
float Kp = 20, Ki = 0, Kd = 8;
float error = 0, lastError = 0, integral = 0;

// Sensor pin
const int sensorPins[5] = {A1, A2, A3, A4, A5};
int sensorValues[5];
int weights[5] = {-2, -1, 0, 1, 2};

void bacaSensor() {
  for (int i = 0; i < 5; i++) {
    sensorValues[i] = analogRead(sensorPins[i]) > 100 ? 1 : 0;
  }
}

int hitungPosisi() {
  int sum = 0, total = 0;
  for (int i = 0; i < 5; i++) {
    sum += sensorValues[i] * weights[i];
    total += sensorValues[i];
  }
  return (total > 0) ? sum : 0; // Jika semua 0, anggap lurus
}

void kontrolPID() {
  int pos = hitungPosisi();
  error = pos;
  integral += error;
  float derivative = error - lastError;
  float output = Kp * error + Ki * integral + Kd * derivative;
  lastError = error;

  int leftSpeed = constrain(baseSpeed - output, 0, maxSpeed);
  int rightSpeed = constrain(baseSpeed + output, 0, maxSpeed);
  motorGerak(leftSpeed, rightSpeed);
}

void motorGerak(int left, int right) {
  digitalWrite(IN1_PIN, left > 0); digitalWrite(IN2_PIN, left <= 0);
  digitalWrite(IN3_PIN, right > 0); digitalWrite(IN4_PIN, right <= 0);
  analogWrite(ENA_PIN, abs(left));
  analogWrite(ENB_PIN, abs(right));
}

void berhenti() {
  analogWrite(ENA_PIN, 0); analogWrite(ENB_PIN, 0);
  digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(ENA_PIN, OUTPUT); pinMode(IN1_PIN, OUTPUT); pinMode(IN2_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT); pinMode(IN3_PIN, OUTPUT); pinMode(IN4_PIN, OUTPUT);
  pinMode(CLP, INPUT_PULLUP);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  berhenti();
}

void aksiCLP() {
  unsigned long start = millis();
  motorGerak(-100, -100); // Mundur sebentar
  delay(1000);
  while (true) {
    bacaSensor();
    if (sensorValues[0] || sensorValues[1] || sensorValues[3] || sensorValues[4]) break;
    motorGerak(-100, -100);
    delay(10);
  }
  berhenti();
}

void loop() {
  if (digitalRead(CLP) == LOW) {
    aksiCLP();
    return;
  }

  bacaSensor();

  // Semua sensor 0 = kemungkinan di api
  if (sensorValues[0] + sensorValues[1] + sensorValues[2] + sensorValues[3] + sensorValues[4] == 0) {
    berhenti();
    if (digitalRead(FLAME_SENSOR_PIN) == HIGH) {
      digitalWrite(FAN_PIN, HIGH);
    } else {
      digitalWrite(FAN_PIN, LOW);
    }
    delay(1000); // Waktu kipas aktif
    motorGerak(-100, -100); // Mundur
    delay(1000);
    while (true) {
      bacaSensor();
      if (sensorValues[0] || sensorValues[1] || sensorValues[3] || sensorValues[4]) break;
      motorGerak(-100, -100);
      delay(10);
    }
    berhenti();
    return;
  }

  kontrolPID(); // Jalankan PID jika normal
  delay(50);
}
