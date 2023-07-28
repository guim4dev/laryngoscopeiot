#include <Arduino.h>
#define LED_BUILTIN 2
#define BUTTON_PIN 21

int lastButtonState = HIGH;
int currentButtonState;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Setup starting...");
  Serial.flush();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Button is connected to GND
  Serial.println("Setup done");
}

void captureButtonClick()
{
  currentButtonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == LOW && currentButtonState == HIGH)
  {
    Serial.print("Initial button state: ");
    Serial.println("Button Released");
  }
  else if (lastButtonState == HIGH && currentButtonState == LOW)
    Serial.println("Button Pressed");

  lastButtonState = currentButtonState;
}

void loop()
{
  captureButtonClick();
  Serial.flush();
}