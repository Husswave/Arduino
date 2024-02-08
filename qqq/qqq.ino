#include <LiquidCrystal.h>

// Define sensor pin
const int SENSOR_PIN = A1;

// Define relay pin
const int RELAY_PIN = 8;

// Define threshold value (adjust as needed)
const int THRESHOLD_VALUE = 140000;

// Define LCD pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // Initialize relay pin as an OUTPUT
  pinMode(RELAY_PIN, OUTPUT);

  // Initialize LCD
  lcd.begin(16, 2);

  // Serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read sensor value
  int sensorValue = analogRead(SENSOR_PIN);

  // Print sensor value for debugging
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  // Display sensor value on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sensor Value:");
  lcd.setCursor(0, 1);
  lcd.print(sensorValue);

  // Check if sensor value is above the threshold
  if (sensorValue > THRESHOLD_VALUE) {
    // Activate relay
    activateRelay();

    // Display relay status on LCD
    lcd.setCursor(0, 0);
    lcd.print("Relay: Activated");
  } else {
    // Deactivate relay
    deactivateRelay();

    // Display relay status on LCD
    lcd.setCursor(0, 0);
    lcd.print("Relay: Deactivated");
  }

  // Delay for a short duration
  delay(1000); // Adjust delay as needed
}

void activateRelay() {
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Relay Activated");
}

void deactivateRelay() {
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Relay Deactivated");
}
