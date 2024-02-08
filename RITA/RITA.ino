#include <LiquidCrystal.h>

const int buttonPin = 8;           // Pin connected to the push button
const int sensorPin = 6;          // Pin connected to the GUR03 ultrasonic sensor
const int ledPin = 9;             // Pin connected to the LED
const int speakerPin = 7;         // Pin connected to the speaker

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);   // Pins connected to the LM016L LCD

int count = 0;                  // Counter variable
bool previousButtonState = false;   // Previous state of the push button

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
  bool buttonState = digitalRead(buttonPin);

  // Check if the button state has changed
  if (buttonState != previousButtonState) {
    if (buttonState == LOW) {
      // Start or resume counting
      count++;
      lcd.clear();
      lcd.print("Product Count: ");
      lcd.setCursor(0, 1);
      lcd.print(count);
    } else {
      // Button opened, stop counting and reset count
      count = 0;
      lcd.clear();
      lcd.print("Product Count: ");
      lcd.setCursor(0, 1);
      lcd.print(count);
    }

    // Blink the LED
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);

    // Play a sound on the speaker
    tone(speakerPin, 1000, 200);
  }

  previousButtonState = buttonState;

  // Delay to avoid counting too rapidly
  delay(100);
}
