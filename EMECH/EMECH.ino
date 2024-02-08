#include <LiquidCrystal.h>

// Define ACS712 sensitivity (mV/A)
const float ACS712_SENSITIVITY = 185.0;

// Define nominal voltage (V)
const float NOMINAL_VOLTAGE = 240.0;

// Define relay threshold (adjust as needed)
const float RELAY_THRESHOLD = 5.0; // Example threshold in Amperes

// Define relay pins
const int RELAY_1_PIN = 8;
const int RELAY_2_PIN = 9;

// Define ACS712 sensor pins
const int MAIN_METER_PIN = A0;
const int SUB_METER_1_PIN = A2;
const int SUB_METER_2_PIN = A1;

// Define LCD pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2);

  // Initialize relay pins
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);

  // Initialize ACS712 sensor pins
  pinMode(MAIN_METER_PIN, INPUT);
  pinMode(SUB_METER_1_PIN, INPUT);
  pinMode(SUB_METER_2_PIN, INPUT);
}

void loop() {
  // Read current from ACS712 sensors
  float mainMeterCurrent = readCurrent(MAIN_METER_PIN);
  float subMeter1Current = readCurrent(SUB_METER_1_PIN);
  float subMeter2Current = readCurrent(SUB_METER_2_PIN);

  // Display current readings on LCD
  lcd.clear();
  lcd.print("Main Meter: ");
  lcd.print(calculatePower(mainMeterCurrent));
  lcd.print(" KVA");
  lcd.setCursor(0, 1);
  lcd.print("Sub Meter 1: ");
  lcd.print(calculatePower(subMeter1Current));
  lcd.print(" KVA");

  // Activate relays based on threshold
  if (mainMeterCurrent > RELAY_THRESHOLD) {
    activateRelay(RELAY_1_PIN);
  } else {
    deactivateRelay(RELAY_1_PIN);
  }

  if (subMeter1Current > RELAY_THRESHOLD) {
    activateRelay(RELAY_2_PIN);
  } else {
    deactivateRelay(RELAY_2_PIN);
  }

  delay(1000); // Adjust delay as needed
}

float readCurrent(int sensorPin) {
  int sensorValue = analogRead(sensorPin);
  float voltage = sensorValue * (5.0 / 1023.0);
  float current = (voltage - (NOMINAL_VOLTAGE / 2.0)) / ACS712_SENSITIVITY;
  return abs(current);
}

float calculatePower(float current) {
  return (NOMINAL_VOLTAGE * current) / 1000.0; // Power in KVA
}

void activateRelay(int relayPin) {
  digitalWrite(relayPin, HIGH);
}

void deactivateRelay(int relayPin) {
  digitalWrite(relayPin, LOW);
}
