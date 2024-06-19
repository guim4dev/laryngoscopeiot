#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

#define LED_BUILTIN 4

String discordWebhookUrl = "http://windmill.x.guima.digital/api/w/admins/jobs/run/h/fb8aaa991d3a5b61?token=9WKmNJSBSeEgVCR1iq7D2taq3Ou3QXjl";

void notifyDiscordWebhook(String content)
{
    HTTPClient http;
    WiFiClient client;

    http.begin(client, discordWebhookUrl);
    http.addHeader("Content-Type", "application/json");

    String json = "{\"payload\":{\"content\":\"" + content + "\"}}";
    int httpCode = http.POST(json);
    http.end();

    Serial.println("Notified Discord webhook with content: " + content);
    Serial.println("Discord webhook response: " + String(httpCode));
    Serial.flush();
}

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