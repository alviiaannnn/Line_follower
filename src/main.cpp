#include <HardwareSerial.h>
#include <pins_arduino.h>
#include <Arduino.h>

// SENSOR GARIS TO DIGITAL
const int SENSOR1 = A1; // ujung kiri
const int SENSOR2 = A2; // kiri
const int SENSOR3 = A3; // tengah
const int SENSOR4 = A4; // kanan
const int SENSOR5 = A5; // ujung kanan

// PIN MOTOR IN TO PWM
const int MOTOR_KIRI = 9;  
const int MOTOR_KANAN = 10;

// SENSOR API
const int SENSOR_API = 8; // misal digital

// KIPAS
const int KIPAS = 11; // pwm

// PID constraint
float Kp = 1.0;
float Ki = 0.1;
float Kd = 0.5;

float error, previousError = 0;
float integral = 0;
float derivative;
float output;

// Setup
void setup() {
    Serial.begin(9600);

    // Sensor garis
    pinMode(SENSOR1, INPUT);
    pinMode(SENSOR2, INPUT);
    pinMode(SENSOR3, INPUT);
    pinMode(SENSOR4, INPUT);
    pinMode(SENSOR5, INPUT);
    
    // Motor
    pinMode(MOTOR_KIRI, OUTPUT);
    pinMode(MOTOR_KANAN, OUTPUT);
    
    // Kipas
    pinMode(KIPAS, OUTPUT);
    
    // Sensor api
    pinMode(SENSOR_API, INPUT);
}

// Fungsi menghitung error dari posisi garis
int calculateError() {
    int sensor1 = digitalRead(SENSOR1);
    int sensor2 = digitalRead(SENSOR2);
    int sensor3 = digitalRead(SENSOR3);
    int sensor4 = digitalRead(SENSOR4);
    int sensor5 = digitalRead(SENSOR5);
