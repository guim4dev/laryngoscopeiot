#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"          //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems

#include <Utils.h>
#include <WebServer.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define TEETH_BUTTON_PIN 2
#define TONGUE_FORCE_SENSOR_PIN 33

void setupTeethButton()
{
    pinMode(TEETH_BUTTON_PIN, INPUT_PULLUP); // Button is connected to GND
}

struct SensorsCapture
{
    int tongueForce = 0;
    int teethPressed = 0;
} sensorsCapture;

void captureSensorsValues()
{
    int tongueForceValue = analogRead(TONGUE_FORCE_SENSOR_PIN);
    int teethSensorValue = !digitalRead(TEETH_BUTTON_PIN);
    sensorsCapture.tongueForce = tongueForceValue;
    sensorsCapture.teethPressed = teethSensorValue;
}

AsyncEventSource sensorEvents("/sensors");

unsigned long sensors_delay = 300; // 0.3 second delay
bool connectedToSensors = false;

void setupSensorsOnWebServer(AsyncWebServer &server)
{
    sensorEvents.onConnect([](AsyncEventSourceClient *client)
                           {
                               client->send("connected", NULL, millis(), 10000);
                               connectedToSensors = true; });

    server.addHandler(&sensorEvents);
}

void setupSensors(AsyncWebServer &server)
{
    setupTeethButton();
    setupSensorsOnWebServer(server);
}

int last_time_sensors = 0;
void sensorsLoopHandler()
{
    if (!connectedToSensors)
    {
        return;
    }

    int now = millis();
    if (now - last_time_sensors < sensors_delay)
    {
        return;
    }

    captureSensorsValues();
    String sensorData = "{\"tongueForce\": " + String(sensorsCapture.tongueForce) + ", \"teethPressed\": " + String(sensorsCapture.teethPressed) + ", \"timestamp\": " + String(now) + " }";
    sensorEvents.send(sensorData.c_str(), "sensorData", now);
    last_time_sensors = now;
}

void sensorsResetHandler()
{
    connectedToSensors = false;
}