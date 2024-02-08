#include <LiquidCrystal.h>

const int buttonPin = 8;           // Pin connected to the push button
const int sensorPin = 9;          // Pin connected to the GUR03 ultrasonic sensor
const int ledPin = 13;             // Pin connected to the LED
const int speakerPin = 12;         // Pin connected to the speaker

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);   // Pins connected to the LM016L LCD

int count = 0;                  // Counter variable
bool buttonState = false;       // State of the push button

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Product Count: ");
  lcd.setCursor(0, 1);
  lcd.print(count);

  delay(1000);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // Check if the button is ON (LOW state due to INPUT_PULLUP)
  if (!buttonState) {
    count++;    // Increment the count

    // Update the LCD display
    lcd.clear();
    lcd.print("Product Count: ");
    lcd.setCursor(0, 1);
    lcd.print(count);

    // Blink the LED
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);

    // Play a sound on the speaker
    tone(speakerPin, 1000, 200);
  }

  // Delay to avoid counting too rapidly
  delay(50);
}
