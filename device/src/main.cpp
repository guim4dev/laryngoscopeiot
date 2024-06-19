#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"          //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems

#include <Camera.h>
#include <Sensors.h>
#include <Utils.h>

// Replace with your network credentials
const char *ssid = "laringoscopiot";
const char *password = "senhasecreta";

void setupWifi()
{
    // // Configures static IP address
    // IPAddress local_IP(192, 168, 0, 222);
    // IPAddress gateway(192, 168, 0, 1);
    // IPAddress subnet(255, 255, 255, 0);
    // while (!WiFi.config(local_IP, gateway, subnet)) {
    //   Serial.println("STA Failed to configure");
    //   digitalWrite(LED_BUILTIN, HIGH);
    //   safeDelay(2000);
    //   digitalWrite(LED_BUILTIN, LOW);
    //   Serial.print(".");
    // }

    // Wi-Fi connection
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        blinkNTimes(1);
        Serial.print(".");
    }

    // own Wifi server mode
    // WiFi.mode(WIFI_AP);
    // WiFi.softAP(ssid, password);

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    notifyDiscordWebhook("Device connected to WiFi with IP: " + WiFi.localIP().toString());
}

// CPU loggers
long ctr0 = 0;
void id0(void *z)
{
    while (1)
    {
        ctr0++;
        safeDelay(10);
    }
}

long ctr1 = 0;
void id1(void *z)
{
    while (1)
    {
        ctr1++;
        safeDelay(10);
    }
}

void ctrLoggerTask(void *z)
{
    while (1)
    {
        Serial.println("CPU 0 CTR: " + String(ctr0));
        Serial.println("CPU 1 CTR: " + String(ctr1));
        Serial.flush();
        safeDelay(3000); // every 3 seconds
    }
}

void startCpuDebug()
{
    Serial.println("Starting CPU debug tasks...");
    xTaskCreatePinnedToCore(id0, "id0", 4096, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(id1, "id1", 4096, NULL, 0, NULL, 1);
    xTaskCreate(ctrLoggerTask, "ctrLoggerTask", 4096, NULL, 0, NULL);
}

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    blinkNTimes(3);
    Serial.println("Setup starting...");
    // startCpuDebug();

    Serial.println("Initial Free PSRAM: " + String(ESP.getFreePsram()));
    setupWifi();

    setupCamera();
    setupSensors();

    Serial.println("Setup done");
    Serial.println("Total heap: " + String(ESP.getHeapSize()));
    Serial.println("Free heap: " + String(ESP.getFreeHeap()));
    Serial.println("Total PSRAM: " + String(ESP.getPsramSize()));
    Serial.println("Free PSRAM: " + String(ESP.getFreePsram()));
    Serial.flush();
}

// What todo with the loop ?
// TODO: move sensors to separate file
void loop()
{
    sensorsLoopHandler();
}