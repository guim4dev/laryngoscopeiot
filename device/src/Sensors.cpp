#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"          //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems

#include <Utils.h>
#include <WebServer.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define TEETH_BUTTON_PIN 2
#define PRIMARY_FORCE_SENSOR_PIN 12
#define SECONDARY_FORCE_SENSOR_PIN 13

void setupTeethButton()
{
    pinMode(TEETH_BUTTON_PIN, INPUT_PULLUP); // Button is connected to GND
}

struct SensorsCapture
{
    int primaryForce = 0;
    int secondaryForce = 0;
    int teethPressed = 0;
} sensorsCapture;

void captureSensorsValues()
{
    // int primaryForceValue = analogRead(PRIMARY_FORCE_SENSOR_PIN);
    // int secondaryForceValue = analogRead(SECONDARY_FORCE_SENSOR_PIN);
    // Serial.print("Secondary force sensor value: ");
    // Serial.println(secondaryForceValue);
    // TMP: get random value between 0 and 4095
    int primaryForceValue = random(4095);
    int secondaryForceValue = random(4095);
    int teethSensorValue = !digitalRead(TEETH_BUTTON_PIN);
    // Serial.print("Primary force sensor value: ");
    // Serial.println(primaryForceValue);

    sensorsCapture.primaryForce = primaryForceValue;
    sensorsCapture.secondaryForce = secondaryForceValue;
    sensorsCapture.teethPressed = teethSensorValue;
}

AsyncWebServer eventsServer(80);
AsyncEventSource sensorEvents("/sensors");

unsigned long sensors_delay = 500; // 0.5 second delay
bool connectedToSensors = false;

// OLD implementation - not used anymore (for now)
void sensorsTask(void *z)
{
    connectedToSensors = true;
    while (1)
    {
        safeDelay(sensors_delay);
        captureSensorsValues();
        String sensorData = "{\"primaryForce\": " + String(sensorsCapture.primaryForce) + ", \"secondaryForce\": " + String(sensorsCapture.secondaryForce) + ", \"teethPressed\": " + String(sensorsCapture.teethPressed) + " }";
        sensorEvents.send(sensorData.c_str(), "sensorData", millis());
    }
};

void setupEventsServer()
{
    prepareWebserver(eventsServer);
    sensorEvents.onConnect([](AsyncEventSourceClient *client)
                           {
                               if (client->lastId())
                               {
                                   Serial.printf("Client reconnected to sensorEvents! Last message ID that it got is: %u\n", client->lastId());
                               }

                               client->send("connected", NULL, millis(), 10000);
                               connectedToSensors = true;
                               // if (!connectedToSensors)
                               // {
                               //     xTaskCreatePinnedToCore(sensorsTask, "sensorsTask", 4096, NULL, 10, NULL, 1);
                               // };
                           });

    eventsServer.addHandler(&sensorEvents);
    eventsServer.begin();
    Serial.println("Events server started");
}

void setupSensors()
{
    setupTeethButton();
    setupEventsServer();

    Serial.println("Sensors setup done");
}

// What todo with the loop ?
// TODO: move sensors to separate file
int last_time_sensors = 0;

void sensorsLoopHandler()
{
    if (!connectedToSensors)
    {
        return;
    }

    if (millis() - last_time_sensors < sensors_delay)
    {
        return;
    }

    captureSensorsValues();
    String sensorData = "{\"primaryForce\": " + String(sensorsCapture.primaryForce) + ", \"secondaryForce\": " + String(sensorsCapture.secondaryForce) + ", \"teethPressed\": " + String(sensorsCapture.teethPressed) + " }";
    sensorEvents.send(sensorData.c_str(), "sensorData", millis());
    last_time_sensors = millis();
}