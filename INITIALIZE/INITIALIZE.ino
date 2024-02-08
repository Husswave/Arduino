#include <SoftwareSerial.h>

SoftwareSerial gsmSerial(1, 0);

void setup() {
  // Initialize GSM communication
  gsmSerial.begin(9600);
  delay(10);    // Allow time for GSM module to initialize
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  Serial.println("GSM Module Initialized");
}

void loop() {
  // Empty loop, no additional functionality
}
