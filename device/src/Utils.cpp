#include <Arduino.h>
#include <WiFi.h>

#define LED_BUILTIN 4

void safeDelay(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

bool setupedBuiltinLed = false;

void setupBuiltinLed()
{
    if (setupedBuiltinLed)
    {
        return;
    }
    pinMode(LED_BUILTIN, OUTPUT);
    setupedBuiltinLed = true;
}

void blinkNTimes(int n)
{
    setupBuiltinLed();
    for (int i = 0; i < n; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        safeDelay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        if (i < n - 1)
        {
            safeDelay(1000); // only delay if it's not the last iteration
        }
    }
}