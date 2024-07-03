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
    cameraResetHandler();
    sensorsResetHandler();
};

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector

    prepareWebserver(webServer, resetHandler);
    setupNetwork();
    setupCamera(webServer);
    setupSensors(webServer);

    // Begin server after setting both camera and sensors
    webServer.begin();

    // Turn on builtin LED
    turnOnBuiltinLed();
}

void loop()
{
    sensorsLoopHandler();
}