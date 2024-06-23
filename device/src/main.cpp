#include <Arduino.h>
#include <functional>
#include "soc/soc.h"          //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems

#include <WebServer.h>
#include <Camera.h>
#include <Sensors.h>
#include <Utils.h>
#include <Network.h>

AsyncWebServer webServer = AsyncWebServer(80);

std::function<void()> resetHandler = []()
{
    Serial.println("Inside reset handler");
    Serial.flush();
    cameraResetHandler();
    Serial.println("Camera resetted");
    Serial.flush();
    sensorsResetHandler();
    Serial.println("Sensors resetted");
    Serial.flush();
};

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    blinkNTimes(3);
    Serial.println("Setup starting...");
    Serial.flush();

    prepareWebserver(webServer, resetHandler);

    setupNetwork();
    setupCamera(webServer);
    setupSensors(webServer);

    // Begin server after setting both camera and sensors
    webServer.begin();

    Serial.println("Setup done");
    Serial.flush();
}

void loop()
{
    sensorsLoopHandler();
}