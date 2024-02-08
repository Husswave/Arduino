#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial gsmSerial(1, 0);

int adcValue;
int analogInputPin = A2;

const int temperatureSensorPin = A0;
const int currentSensorPin = A1;
const int ultrasonicTrigPin = 9;
const int ultrasonicEchoPin = 10;
const int relayPin = 13;

const float currentThreshold = 185.0;
const float voltageThreshold = 4500.0;
const float temperatureThreshold = 100.0;
const float oilLevelThreshold = 40.0;
const float loadingLevelThreshold = 95.0;

bool relayState = HIGH;

bool currentFault = false;
bool voltageFault = false;
bool temperatureFault = false;
bool oilLevelFault = false;
bool loadingLevelFault = false;

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(ultrasonicTrigPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);

  lcd.begin(16, 2);
  lcd.print("Transformer Monitor");
  lcd.setCursor(0, 1);
  lcd.print("and Control System");
  delay(100);
  lcd.clear();

  gsmSerial.begin(9600);
  delay(10);
  sendSMS("System initialized");
}

void loop() {
  digitalWrite(relayPin, LOW);
  relayState = LOW;

  float current = readCurrentSensor();
  float voltage = readVoltageSensor();
  float temperature = readTemperatureSensor();
  float oilLevel = readUltrasonicSensor();
  float loadingLevel = calculateLoadingLevel(current);

  currentFault = current > currentThreshold;
  voltageFault = voltage < voltageThreshold;
  temperatureFault = temperature > temperatureThreshold;
  oilLevelFault = oilLevel < oilLevelThreshold;
  loadingLevelFault = loadingLevel > loadingLevelThreshold;

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

  if (currentFault) {
    lcd.clear();
    lcd.print("Over-Current");
    sendSMS("NO FAULT DETECTED");
    delay(1000);
  }

  if (voltageFault) {
    lcd.clear();
    lcd.print("Low Voltage");
    sendSMS("NO FAULT DETECTED");
    delay(1000);
  }

  if (temperatureFault) {
    lcd.clear();
    lcd.print("High Temperature");
    sendSMS("NO FAULT DETECTED");
    delay(1000);
  }

  if (oilLevelFault) {
    lcd.clear();
    lcd.print("Low Oil Level");
    sendSMS("NO FAULT DETECTED");
    delay(1000);
  }

  if (loadingLevelFault) {
    lcd.clear();
    lcd.print("Overloading");
    sendSMS("NO FAULT DETECTED");
    delay(1000);
  }

  if (!currentFault && !loadingLevelFault && !voltageFault && !temperatureFault && !oilLevelFault) {
    lcd.clear();
    lcd.print("NO FAULT DETECTED");
    delay(1000);
  }

  if (currentFault || loadingLevelFault || voltageFault) {
    if (relayState == LOW) {
      digitalWrite(relayPin, HIGH);
      relayState = HIGH;
    }
  } else {
    if (relayState == HIGH) {
      digitalWrite(relayPin, LOW);
      relayState = LOW;
    }
  }

  if (!currentFault && !loadingLevelFault) {
    if (relayState == LOW) {
      digitalWrite(relayPin, HIGH);
      relayState = HIGH;
    }
  }

  delay(100);
}

void sendSMS(const char* message) {
  gsmSerial.println("AT+CMGF=1");
  delay(10);
  gsmSerial.println("AT+CMGS=\"PERSONEL INCHARGE\"");
  delay(10);
  gsmSerial.println(message);
  delay(10);
  gsmSerial.println((char)26);
  delay(50);
}

float readCurrentSensor() {
  int rawValue = analogRead(currentSensorPin);
  float voltage = rawValue * (5.0 / 1024.0);
  float current = voltage * 6.92 / 0.185;
  return current;
}

float readUltrasonicSensor() {
  digitalWrite(ultrasonicTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicTrigPin, LOW);

  long duration = pulseIn(ultrasonicEchoPin, HIGH);
  float distance = duration * 0.034 / 2.0;

  return distance;
}

float readTemperatureSensor() {
  int rawValue = analogRead(temperatureSensorPin);
  float voltage = rawValue * (5.0 / 1024.0);
  float temperature = voltage * 100.0;
  return temperature;
}

float readVoltageSensor() {
  int rawValue = analogRead(analogInputPin);
  float voltage = rawValue * 6000.0 / 1023.0;

  return voltage;
}

float calculateLoadingLevel(float current) {
  float loadingLevel = (current / currentThreshold) * 100.0;
  return loadingLevel;
}
