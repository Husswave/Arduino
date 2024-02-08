#include <LiquidCrystal.h>      // Include the LCD library
#include <SoftwareSerial.h>     // Include the SoftwareSerial library for GSM communication

SoftwareSerial gsmSerial(1, 0);  // SoftwareSerial object for GSM communication

int adcValue;                    // Declaration of the ADC value variable
int analogInputPin = A2;         // Declare the pin to read the AC voltmeter reading

// Pin assignments
const int temperatureSensorPin = A0;
const int currentSensorPin = A1;
const int ultrasonicTrigPin = 9;
const int ultrasonicEchoPin = 10;
const int relayPin = 13;

// Threshold values
const float currentThreshold = 185.0;    // Adjust this value as per your requirements
const float voltageThreshold = 4500.0;   // Adjust this value as per your requirements
const float temperatureThreshold = 100.0; // Adjust this value as per your requirements
const float oilLevelThreshold = 40.0;    // Adjust this value as per your requirements
const float loadingLevelThreshold = 95.0; // Adjust this value as per your requirements

// Relay state
bool relayState = HIGH;        // Initial state of the relay (ON)

// Fault flags
bool currentFault = false;
bool voltageFault = false;
bool temperatureFault = false;
bool oilLevelFault = false;
bool loadingLevelFault = false;

// LCD display setup
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  // Change the pin numbers to match your connections

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(ultrasonicTrigPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);

  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.print("Transformer Monitor");
  lcd.setCursor(0, 1);
  lcd.print("and Control System");
  delay(100);
  lcd.clear();

  // Initialize GSM communication
  gsmSerial.begin(9600);
  delay(10);    // Allow time for GSM module to initialize
  sendSMS("System initialized");
}

void loop() {
  // Activate relay
  digitalWrite(relayPin, LOW);
  relayState = LOW;

  // Read sensor values
  float current = readCurrentSensor();
  float voltage = readVoltageSensor();
  float temperature = readTemperatureSensor();
  float oilLevel = readUltrasonicSensor();
  float loadingLevel = calculateLoadingLevel(current);

  // Check if current or loading level exceeds the thresholds
  currentFault = current > currentThreshold;
  voltageFault = voltage < voltageThreshold;
  temperatureFault = temperature > temperatureThreshold;
  oilLevelFault = oilLevel < oilLevelThreshold;
  loadingLevelFault = loadingLevel > loadingLevelThreshold;

  // Display sensor values on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current: ");
  lcd.print(current);
  lcd.print(" A");

  lcd.setCursor(0, 1);
  lcd.print("Voltage: ");
  lcd.print(voltage);
  lcd.print(" V");

  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Oil Lvl: ");
  lcd.print(oilLevel);
  lcd.print(" cm");

  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loading %: ");
  lcd.print(loadingLevel);
  lcd.print(" %");

  delay(1000);

  // Check if there are any faults in the parameters
  if (currentFault) {
    lcd.clear();
    lcd.print("Over-Current");
    sendSMS("Over-Current fault detected"); // Send SMS alert
    delay(1000);
  }

  if (voltageFault) {
    lcd.clear();
    lcd.print("Low Voltage");
    sendSMS("FAULT(S) DETECTED"); // Send SMS alert
    delay(1000);
  }

  if (temperatureFault) {
    lcd.clear();
    lcd.print("High Temperature");
    sendSMS("FAULT(S) DETECTED"); // Send SMS alert
    delay(1000);
  }

  if (oilLevelFault) {
    lcd.clear();
    lcd.print("Low Oil Level");
    sendSMS("FAULT(S) DETECTED"); // Send SMS alert
    delay(1000);
  }

  if (loadingLevelFault) {
    lcd.clear();
    lcd.print("Overloading");
    sendSMS("FAULT(S) DETECTED"); // Send SMS alert
    delay(1000);
  }

  // Activate/deactivate relay based on faults
  if (currentFault || loadingLevelFault || voltageFault) {
    if (relayState == LOW) {
      digitalWrite(relayPin, HIGH);  // Turn on the relay
      relayState = HIGH;
    }
  } else {
    if (relayState == HIGH) {
      digitalWrite(relayPin, LOW);   // Turn off the relay
      relayState = LOW;
    }
  }

  // Check if there are no faults in current and loading level
  if (!currentFault && !loadingLevelFault) {
    // Return relay to original state
    if (relayState == LOW) {
      digitalWrite(relayPin, HIGH);
      relayState = HIGH;
    }
  }

  delay(100); // Add a small delay for stability
}

// Function to send SMS
void sendSMS(const char* message) {
  gsmSerial.println("AT+CMGF=1");   // Set the GSM module to text mode
  delay(10);
  gsmSerial.println("AT+CMGS=\"PERSONEL INCHARGE\"");   // Replace with your destination phone number
  delay(10);
  gsmSerial.println(message);
  delay(10);
  gsmSerial.println((char)26);       // End of message character
  delay(50);
}

// Rest of the code including sensor reading and calculation functions
float readCurrentSensor() {
  // Read ACS712 current sensor and convert to current
  int rawValue = analogRead(currentSensorPin);
  float voltage = rawValue * (5.0 / 1024.0);   // Convert raw value to voltage
  float current = voltage * 6.92 / 0.185;       // Convert voltage to current based on ACS712 sensitivity
  return current;
}

float readUltrasonicSensor() {
  // Send a trigger pulse
  digitalWrite(ultrasonicTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicTrigPin, LOW);
  // Measure the echo pulse duration
  long duration = pulseIn(ultrasonicEchoPin, HIGH);

  // Convert the duration to distance (cm)
  float distance = duration * 0.034 / 2.0;

  return distance;
}

float readTemperatureSensor() {
  // Read LM35 temperature sensor and convert to Celsius
  int rawValue = analogRead(temperatureSensorPin);
  float voltage = rawValue * (5.0 / 1024.0);   // Convert raw value to voltage
  float temperature = voltage * 100.0;          // Convert voltage to temperature in Celsius
  return temperature;
}

float readVoltageSensor() {
  int rawValue = analogRead(analogInputPin);    // Read analog input value
  float voltage = rawValue * 6000.0 / 1023.0;   // Convert to voltage (assuming a 5V reference)

  return voltage;
}

float calculateLoadingLevel(float current) {
  // Calculate loading level based on current
  float loadingLevel = (current / currentThreshold) * 100.0;
  return loadingLevel;
}
