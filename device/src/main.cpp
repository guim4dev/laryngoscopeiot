#include <Arduino.h>
#define LED_BUILTIN 4

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Setup starting...");
  Serial.flush();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("Waiting 5s...");
  Serial.flush();
  delay(5000);
}