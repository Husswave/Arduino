#include <LiquidCrystal.h>

// Define ACS712 sensitivity (mV/A)
#define ACS712_SENSITIVITY 185.0

// Define nominal voltage (V)
#define NOMINAL_VOLTAGE 240.0

// Define ACS712 sensor pins
#define MAIN_METER_PIN A1
#define HOUSE_A_PIN A2
#define HOUSE_B_PIN A5

// Define relay pins
#define RELAY_1_PIN A3
#define RELAY_2_PIN A4

// Define relay thresholds in mVA (adjust as needed)
#define RELAY_THRESHOLD_KVA_1 5000 // Adjusted threshold for RELAY_1 in KVA
#define RELAY_THRESHOLD_KVA_2 6000 // Adjusted threshold for RELAY_2 in KVA

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

  // Set relay pins to LOW initially (assuming active-high relays)
  digitalWrite(RELAY_1_PIN, LOW);
  digitalWrite(RELAY_2_PIN, LOW);

  // Initialize ACS712 sensor pins
  pinMode(MAIN_METER_PIN, INPUT);
  pinMode(HOUSE_A_PIN, INPUT);
  pinMode(HOUSE_B_PIN, INPUT);
}

void loop() {
  // Read current from ACS712 sensors
  float houseACurrent = readCurrent(HOUSE_A_PIN);
  float houseBCurrent = readCurrent(HOUSE_B_PIN);

  Serial.print("House A Current: ");
  Serial.println(houseACurrent);
  Serial.print("House B Current: ");
  Serial.println(houseBCurrent);

  // Display individual house readings on the first screen
  lcd.clear();
  lcd.print("House A: ");
  lcd.print(calculatePower(houseACurrent));
  lcd.print(" mVA");

  lcd.setCursor(0, 1);
  lcd.print("House B: ");
  lcd.print(calculatePower(houseBCurrent));
  lcd.print(" mVA");

  // Delay for a while
  delay(5000); // Reduce this delay if you want more frequent updates

  // Display the sum of house readings at the main meter on the second screen
  lcd.clear();
  lcd.print("Main Meter: ");
  float mainMeterCurrent = houseACurrent + houseBCurrent;
  lcd.print(calculatePower(mainMeterCurrent));
  lcd.print(" mVA");

  lcd.setCursor(0, 1);
  lcd.print("Sum: ");
  lcd.print(calculatePower(mainMeterCurrent));
  lcd.print(" mVA");

  // Activate relays based on thresholds
  if (houseACurrent > RELAY_THRESHOLD_KVA_1) {
    activateRelay(RELAY_1_PIN);
  } else {
    deactivateRelay(RELAY_1_PIN);
  }

  if (houseBCurrent > RELAY_THRESHOLD_KVA_2) {
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
  return (NOMINAL_VOLTAGE * current * 1000.0); // Power in mVA
}

void activateRelay(int relayPin) {
  if (digitalRead(relayPin) == LOW) {
    digitalWrite(relayPin, HIGH);  // Set to HIGH to activate (Normally Closed)
    Serial.println("Relay Activated");
  }
}

void deactivateRelay(int relayPin) {
  if (digitalRead(relayPin) == HIGH) {
    digitalWrite(relayPin, LOW); // Set to LOW to deactivate (Normally Closed)
    Serial.println("Relay Deactivated");
  }
}
