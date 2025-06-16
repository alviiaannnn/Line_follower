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
#define CLP 6
#define NEAR 7 //LOW ketika mendeteksi api
#define FAN_PIN 11
#define FLAME_SENSOR_PIN 13

int maxSpeed = 120;
int baseSpeed = 80;

// PID variables
float Kp = 60, Ki = 0, Kd = 5;
float error = 0, lastError = 0, integral = 0;

// Sensor pin
const int sensorPins[5] = {A1, A2, A3, A4, A5};
int sensorValues[5];
int weights[5] = {-2, -1, 0, 1, 2};

void bacaSensor() {
  for (int i = 0; i < 5; i++) {
    sensorValues[i] = analogRead(sensorPins[i]) > 100 ? 0 : 1;
    // Sensor bernilai 1 jika mendeteksi garis, 0 jika tidak
  }
}

int hitungPosisi() {
  int sum = 0, total = 0;
  for (int i = 0; i < 5; i++) {
    sum += !sensorValues[i] * weights[i];
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

  int leftSpeed = constrain(baseSpeed + output, 0, maxSpeed);
  int rightSpeed = constrain(baseSpeed - output, 0, maxSpeed);
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

void kiriTajam() {
  analogWrite(ENA_PIN, baseSpeed); analogWrite(ENB_PIN, maxSpeed);
  digitalWrite(IN3_PIN, HIGH); digitalWrite(IN4_PIN, LOW);
  digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, HIGH);
}

void kananTajam() {
  analogWrite(ENA_PIN, maxSpeed); analogWrite(ENB_PIN, baseSpeed);
  digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, HIGH);
  digitalWrite(IN1_PIN, HIGH); digitalWrite(IN2_PIN, LOW);
}

void aksiCLP() {
  unsigned long start = millis();
  motorGerak(-baseSpeed, -baseSpeed); // Mundur sebentar
  delay(1000);
  while (millis() - start < 10000) { // maksimal 10 detik sampai mendeteksi garis
    bacaSensor();
    if (sensorValues[0] || sensorValues[1] || sensorValues[3] || sensorValues[4]) break;
    motorGerak(-baseSpeed, -baseSpeed);
    delay(10);
  }
  berhenti();
}

void aksiKipas(bool flameSensor) {
  if (flameSensor) {
    berhenti();
    digitalWrite(FAN_PIN, HIGH); // Nyalakan kipas
  } else {
    digitalWrite(FAN_PIN, LOW); // Matikan kipas
    return;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(ENA_PIN, OUTPUT); pinMode(IN1_PIN, OUTPUT); pinMode(IN2_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT); pinMode(IN3_PIN, OUTPUT); pinMode(IN4_PIN, OUTPUT);
  pinMode(CLP, INPUT_PULLUP);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(NEAR, INPUT);
  berhenti();
}

void loop() {
  if (digitalRead(CLP) == HIGH) {
    aksiCLP();
    return;
  }

  bacaSensor();

  if (!sensorValues[0] || !sensorValues[1] || !sensorValues[2] || !sensorValues[3] || !sensorValues[4]) {
    unsigned long start = millis();
    while (millis() - start < 500) { // Tunggu 0.5 detik
      bacaSensor();
      kontrolPID();
    }
    berhenti();
    delay(1000); // Tunggu 1 detik sebelum aksi kipas
    aksiKipas(!NEAR);

    unsigned long start2 = millis();
    while (millis() - start2 < 5000) { // Tunggu maksimal 10 detik
      bacaSensor();
      if (sensorValues[0] || sensorValues[1] || sensorValues[3] || sensorValues[4]) break;
      motorGerak(-baseSpeed, -baseSpeed);
    }
    return;
  } else if ( sensorValues[0] && sensorValues[1] && sensorValues[2] ) {
      kananTajam();
      while (true) {
        bacaSensor();
        if (sensorValues[3] || sensorValues[4]) break;
      } 
      kontrolPID(); 
      return;
  } else if ( sensorValues[4] && sensorValues[3] && sensorValues[2] ) {
      kiriTajam();
      while (true) {
        bacaSensor();
        if (sensorValues[0] || sensorValues[1]) break;
      }
      kontrolPID(); 
      return;
  }
  kontrolPID(); // Jalankan PID jika normal
}
