#include <Arduino.h>

// Motor A (Kiri)
#define ENA_PIN 9
#define IN1_PIN 2
#define IN2_PIN 3
// Motor B (Kanan)
#define ENB_PIN 10
#define IN3_PIN 4
#define IN4_PIN 5
// CLP
#define CLP 12
// fan
#define FAN_PIN 11
// Flame Sensor
#define FLAME_SENSOR_PIN 12


int baseSpeed = 180; // (MAX = 205)
int speed = baseSpeed + 50; // Kecepatan dasar motor
int slow = baseSpeed - 50;
int verySlow = baseSpeed - 100;

// Fungsi untuk menggerakkan motor
void maju(int speed = baseSpeed) {
  digitalWrite(IN1_PIN, HIGH); digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH); digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, speed); analogWrite(ENB_PIN, speed);
}

void mundur(int speed = baseSpeed) {
  digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, speed); analogWrite(ENB_PIN, speed);
}

void belokKiri(int speedL = slow, int speedR = baseSpeed) {
  digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, HIGH); digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, speedL); analogWrite(ENB_PIN, speedR);
}

void belokKanan(int speedL = baseSpeed, int speedR = slow) {
  digitalWrite(IN1_PIN, HIGH); digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, speedL); analogWrite(ENB_PIN, speedR);
}

void belokKiriTajam(int speedL = verySlow, int speedR = speed) {
  digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, HIGH); digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, speedL); analogWrite(ENB_PIN, speedR);
}

void belokKananTajam(int speedL = speed, int speedR = verySlow) {
  digitalWrite(IN1_PIN, HIGH); digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, speedL); analogWrite(ENB_PIN, speedR);
}

void berhenti() {
  digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, 0); analogWrite(ENB_PIN, 0);
}

void berputar(int speed = slow) {
  // Motor kiri maju, motor kanan mundur (berputar di tempat)
  digitalWrite(IN1_PIN, HIGH); digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, speed); analogWrite(ENB_PIN, speed);
}

static unsigned long allHighStartTime = 0;

void setup() {
  Serial.begin(9600); // Inisialisasi serial untuk debugging
  pinMode(ENA_PIN, OUTPUT); pinMode(IN1_PIN, OUTPUT); pinMode(IN2_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT); pinMode(IN3_PIN, OUTPUT); pinMode(IN4_PIN, OUTPUT);
  pinMode(CLP, INPUT_PULLUP); // CLP sebagai input dengan pull-up internal
  pinMode(FAN_PIN, OUTPUT); // Kipas sebagai output
  pinMode(FLAME_SENSOR_PIN, INPUT); // Flame sensor sebagai input
  
  digitalWrite(FAN_PIN, LOW); // Pastikan kipas mati saat awal
  berhenti(); // Pastikan motor berhenti saat awal
}

void loop() {
  bool S5 = analogRead(A1) >= 100;
  bool S4 = analogRead(A2) >= 100;
  bool S3 = analogRead(A3) >= 100;
  bool S2 = analogRead(A4) >= 100;
  bool S1 = analogRead(A5) >= 100;
  bool clp = digitalRead(CLP) == LOW; // CLP aktif jika LOW

  if (clp) {
    static bool clpActionInProgress = false;
    static unsigned long clpActionStartTime = 0;
    static int clpActionStep = 0;

    if (!clpActionInProgress) {
      clpActionInProgress = true;
      clpActionStartTime = millis();
      clpActionStep = 0;
    }

    if (clpActionInProgress) {
      unsigned long elapsed = millis() - clpActionStartTime;
      switch (clpActionStep) {
      case 0:
        maju();
        if (elapsed >= 1000) {
        berhenti();
        clpActionStep = 1;
        clpActionStartTime = millis();
        }
        break;
      case 1:
        if (elapsed >= 500) {
        // Mundur sampai salah satu dari S1, S2, S4, atau S5 bernilai true
        while (S1 || S2 || S4 || S5) {
          mundur();
          delay(10); // Hindari loop terlalu cepat
        }
        berhenti();
        clpActionStep = 2;
        allHighStartTime = 0; // Reset timer if CLP is pressed
        clpActionInProgress = false;
        }
        break;
      }
      return; // Skip the rest of loop() while action is running
    }
  } else {
    if (S1 && S2 && S3 && S4 && S5) {
      if (allHighStartTime == 0) {
        allHighStartTime = millis();
        maju();
      } else if (millis() - allHighStartTime > 2000) {
        berputar();
      } else {
        maju();
      }
    }
    else if (!S1 && !S2 && !S3 && !S4 && !S5) {
      berhenti();
      delay(500); // Delay untuk berheni  sejenak
      if (digitalRead(FLAME_SENSOR_PIN) == HIGH) {
        // Jika flame sensor mendeteksi api, aktifkan kipas
        digitalWrite(FAN_PIN, HIGH);
      } else {
        // Jika tidak ada api, tetap matikan kipas
        digitalWrite(FAN_PIN, LOW);
      }
      delay(1000); // Delay untuk memastikan kipas menyala
      mundur();
      delay(1000); // Mundur selama 1 detik
      while (true) {
        bool S1 = analogRead(A5) >= 100;
        bool S2 = analogRead(A4) >= 100;
        bool S4 = analogRead(A2) >= 100;
        bool S5 = analogRead(A1) >= 100;
        if (!(S1 || S2 || S4 || S5)) break;
        mundur();
        delay(10);
      }
      berhenti();
    }
    else if (
      (!S1 && S2 && S3 && S4 && S5) ||
      (!S1 && !S2 && S3 && S4 && S5) ||
      (!S1 && !S2 && !S3 && S4 && S5) ||
      (!S1 && !S2 && !S3 && !S4 && S5)
    ) {
      belokKiriTajam();
    }
    else if (
      (S1 && !S2 && S3 && S4 && S5) ||
      (S1 && !S2 && !S3 && S4 && S5)
    ) {
      belokKiri();
    }
    else if (
      (S1 && S2 && !S3 && S4 && S5) ||
      (S1 && !S2 && !S3 && !S4 && S5)
    ) {
      maju();
    }
    else if (
      (S1 && S2 && S3 && !S4 && S5) ||
      (S1 && S2 && !S3 && !S4 && S5)
    ) {
      belokKanan();
    }
    else if (
      (S1 && S2 && S3 && S4 && !S5) ||
      (S1 && S2 && S3 && !S4 && !S5) ||
      (S1 && S2 && !S3 && !S4 && !S5) ||
      (S1 && !S2 && !S3 && !S4 && !S5)
    ) {
      belokKananTajam();
    }
    else {
      // Jika tidak ada kondisi yang terpenuhi, berhenti
      berhenti();
      digitalWrite(FAN_PIN, LOW); // Pastikan kipas mati
      allHighStartTime = 0; // Reset timer if no sensors are active
    }
  }

  delay(100);
}

