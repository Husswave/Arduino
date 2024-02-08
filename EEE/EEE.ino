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

// Global variables to track relay states and house readings
bool relay1Activated = false;
bool relay2Activated = false;
float houseACurrent = 0.0;
float houseBCurrent = 0.0;

void setup() {
  // Initialize serial communication
  Serial.begin(600);

  // Initialize LCD
  lcd.begin(16, 2);

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
  houseACurrent = readCurrent(HOUSE_A_PIN);
  houseBCurrent = readCurrent(HOUSE_B_PIN);

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
  delay(500); // Reduce this delay if you want more frequent updates

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
  if (calculatePower(houseACurrent) > RELAY_THRESHOLD_POWER_1) {
    activateRelay(RELAY_1_PIN, relay1Activated);
  } else {
    deactivateRelay(RELAY_1_PIN, relay1Activated);
  }

  if (calculatePower(houseBCurrent) > RELAY_THRESHOLD_POWER_2) {
    activateRelay(RELAY_2_PIN, relay2Activated);
  } else {
    deactivateRelay(RELAY_2_PIN, relay2Activated);
  }

  delay(600); // Adjust delay as needed
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

void activateRelay(int relayPin, bool& relayActivated) {
  if (!relayActivated) {
    digitalWrite(relayPin, LOW);  // Set to LOW to activate (normally closed)
    relayActivated = true;
    Serial.println("Relay Activated");
    // Reset the corresponding house reading to zero
    if (relayPin == RELAY_1_PIN) {
      houseACurrent = 0.0;
    } else if (relayPin == RELAY_2_PIN) {
      houseBCurrent = 0.0;
    }
  }
  // Add an additional check to deactivate the relay if the threshold is no longer met
  else if (relayPin == RELAY_1_PIN && calculatePower(houseACurrent) <= RELAY_THRESHOLD_POWER_1) {
    deactivateRelay(RELAY_1_PIN, relayActivated);
  } else if (relayPin == RELAY_2_PIN && calculatePower(houseBCurrent) <= RELAY_THRESHOLD_POWER_2) {
    deactivateRelay(RELAY_2_PIN, relayActivated);
  }
}

void deactivateRelay(int relayPin, bool& relayActivated) {
  // Prevent deactivating the relay once it has been activated
  if (relayActivated) {
    return;
  }
  
  digitalWrite(relayPin, HIGH); // Set to HIGH to deactivate (normally closed)
  relayActivated = false;
  Serial.println("Relay Deactivated");
}
