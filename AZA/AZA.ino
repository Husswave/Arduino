#include <LiquidCrystal.h>

// Define ACS712 sensitivity (mV/A)
#define ACS712_SENSITIVITY 185.0

// Define nominal voltage (V)
#define NOMINAL_VOLTAGE 240.0

// Define ACS712 sensor pins
#define MAIN_METER_PIN A0
#define HOUSE_A_PIN A2
#define HOUSE_B_PIN A4

// Define relay pins
#define RELAY_1_PIN 8
#define RELAY_2_PIN 9

// Define relay thresholds (adjust as needed)
#define RELAY_THRESHOLD_POWER_1 150000.0 // Example threshold for RELAY_1 in mVA
#define RELAY_THRESHOLD_POWER_2 155500.0 // Example threshold for RELAY_2 in mVA

// Define LCD pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Persistent variables to track relay activation state and corresponding house readings
bool relay1Activated = false;
bool relay2Activated = false;
float houseACurrent = 0.0;
float houseBCurrent = 0.0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();

  // Initialize relay pins
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);

  // Initialize ACS712 sensor pins
  pinMode(MAIN_METER_PIN, INPUT);
  pinMode(HOUSE_A_PIN, INPUT);
  pinMode(HOUSE_B_PIN, INPUT);
}

void loop() {
  // Read current from ACS712 sensors
  float houseACurrent = readCurrent(HOUSE_A_PIN);
  float houseBCurrent = readCurrent(HOUSE_B_PIN);

  // Display individual house readings on the first screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("House A: ");
  lcd.print(calculatePower(houseACurrent));
  lcd.print(" mVA");

  lcd.setCursor(0, 1);
  lcd.print("House B: ");
  lcd.print(calculatePower(houseBCurrent));
  lcd.print(" mVA");

  // Delay for a while
  delay(500); // Reduce this delay if you want more frequent updates

  // Display the sum of house readings at the main meter on the second screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Main Meter: ");
  float mainMeterCurrent = houseACurrent + houseBCurrent;
  lcd.print(calculatePower(mainMeterCurrent));
  lcd.print(" mVA");

  lcd.setCursor(0, 1);
  lcd.print("Sum: ");
  lcd.print(calculatePower(mainMeterCurrent));
  lcd.print(" mVA");

  // Activate relays based on thresholds
  if (calculatePower(houseACurrent) > RELAY_THRESHOLD_POWER_1 && !relay1Activated) {
    activateRelay(RELAY_1_PIN, houseACurrent, relay1Activated);
  }

  if (calculatePower(houseBCurrent) > RELAY_THRESHOLD_POWER_2 && !relay2Activated) {
    activateRelay(RELAY_2_PIN, houseBCurrent, relay2Activated);
  }

  delay(9600); // Adjust delay as needed
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

void activateRelay(int relayPin, float &houseCurrent, bool &relayActivated) {
  if (digitalRead(relayPin) == HIGH) {
    digitalWrite(relayPin, LOW);  // Set to LOW to activate (normally closed)
    Serial.println("Relay Activated");

    // Reset the corresponding house reading to zero
    houseCurrent = 0.0;

    // Set relay activation state to true
    relayActivated = true;
  }
}
