#include <LiquidCrystal.h>

// Define ACS712 sensitivity (mV/A)
#define ACS712_SENSITIVITY 185.0

// Define nominal voltage (V)
#define NOMINAL_VOLTAGE 240.0

// Define ACS712 sensor pins
#define HOUSE_A_PIN A2
#define HOUSE_B_PIN A1

// Define relay pins
#define RELAY_1_PIN 8
#define RELAY_2_PIN 9

// Define PWM load pins
#define HOUSE_A_LOAD_PIN 6
#define HOUSE_B_LOAD_PIN 5

// Define relay thresholds (adjust as needed)
#define RELAY_THRESHOLD_KVA_1 0.1 // Example threshold for RELAY_1 in KVA
#define RELAY_THRESHOLD_KVA_2 0.2 // Example threshold for RELAY_2 in KVA

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
  pinMode(HOUSE_A_PIN, INPUT);
  pinMode(HOUSE_B_PIN, INPUT);

  // Initialize PWM load pins
  pinMode(HOUSE_A_LOAD_PIN, OUTPUT);
  pinMode(HOUSE_B_LOAD_PIN, OUTPUT);
}

void loop() {
  // Simulate changes in load with random PWM duty cycle for House A
  int randomDutyCycleA = random(256); // Generate a random duty cycle between 0 and 255
  analogWrite(HOUSE_A_LOAD_PIN, randomDutyCycleA);

  // Simulate changes in load with random PWM duty cycle for House B
  int randomDutyCycleB = random(256); // Generate a random duty cycle between 0 and 255
  analogWrite(HOUSE_B_LOAD_PIN, randomDutyCycleB);

  // Display individual house readings on the first screen
  lcd.clear();
  lcd.print("House A: ");
  lcd.print(calculatePower(readCurrent(HOUSE_A_PIN)));
  lcd.print(" KVA");

  lcd.setCursor(0, 1);
  lcd.print("House B: ");
  lcd.print(calculatePower(readCurrent(HOUSE_B_PIN)));
  lcd.print(" KVA");

  // Delay for 2 seconds
  delay(2000);

  // Display the sum of house readings at the main meter on the second screen
  lcd.clear();
  lcd.print("Main Meter: ");
  float mainMeterCurrent = readCurrent(HOUSE_A_PIN) + readCurrent(HOUSE_B_PIN);
  lcd.print(calculatePower(mainMeterCurrent));
  lcd.print(" KVA");

  lcd.setCursor(0, 1);
  lcd.print("Sum: ");
  lcd.print(calculatePower(mainMeterCurrent));
  lcd.print(" KVA");

  // Activate relays based on thresholds
  if (readCurrent(HOUSE_A_PIN) > RELAY_THRESHOLD_KVA_1) {
    activateRelay(RELAY_1_PIN);
  } else {
    deactivateRelay(RELAY_1_PIN);
  }

  if (readCurrent(HOUSE_B_PIN) > RELAY_THRESHOLD_KVA_2) {
    activateRelay(RELAY_2_PIN);
  } else {
    deactivateRelay(RELAY_2_PIN);
  }

  // Delay for 2 seconds
  delay(2000); 
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
  if (digitalRead(relayPin) == HIGH) {
    digitalWrite(relayPin, LOW);  // Set to LOW to activate (normally closed)
    Serial.println("Relay Activated");
  }
}

void deactivateRelay(int relayPin) {
  if (digitalRead(relayPin) == LOW) {
    digitalWrite(relayPin, HIGH); // Set to HIGH to deactivate (normally closed)
    Serial.println("Relay Deactivated");
  }
}
