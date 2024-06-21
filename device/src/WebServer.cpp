#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

static const char *PLAIN_TEXT_CONTENT_TYPE = "text/plain";

bool appliedGlobalConfigs = false;

void applyServerDefaults(AsyncWebServer &server)
{
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
        if (request->method() == HTTP_OPTIONS)
        {
            request->send(200);
        }
        else
        {
            request->send(404);
        } });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, PLAIN_TEXT_CONTENT_TYPE, "alive"); });
};

void applyGlobalConfigsIfNeeded()
{
    if (appliedGlobalConfigs)
    {
        return;
    }
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    appliedGlobalConfigs = true;
}

void prepareWebserver(AsyncWebServer &server)
{
    applyGlobalConfigsIfNeeded();
    applyServerDefaults(server);
}