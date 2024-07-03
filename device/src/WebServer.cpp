#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <functional>

#include <Utils.h>

static const char *PLAIN_TEXT_CONTENT_TYPE = "text/plain";

bool appliedGlobalConfigs = false;

AsyncWebServer *globalServer = NULL;
std::function<void()> globalServerResetCallback = NULL;

void resetServerTask(void *z)
{
    safeDelay(1000);     // wait one second before resetting the server
    globalServer->end(); // end the server for a clean restart
    if (globalServerResetCallback != NULL)
    {
        globalServerResetCallback(); // call the callback to reset the server if needed
    }
    safeDelay(1000);       // wait one second for the server to end
    globalServer->begin(); // begin the server again
    vTaskDelete(NULL);     // delete the task
}

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
    server.on("/laringoscopiot/status", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, PLAIN_TEXT_CONTENT_TYPE, "alive"); });
    server.on("/laringoscopiot/reset", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                xTaskCreatePinnedToCore(resetServerTask, "resetServerTask", 4096, NULL, 1, NULL, 1);
                request->send(200, PLAIN_TEXT_CONTENT_TYPE, "resetting"); });
};

void applyGlobalConfigsIfNeeded()
{
    if (appliedGlobalConfigs)
    {
        return;
    }
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader("Vary", "*");
    DefaultHeaders::Instance().addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    appliedGlobalConfigs = true;
}

void prepareWebserver(AsyncWebServer &server, std::function<void()> resetServerCallBack)
{
    applyGlobalConfigsIfNeeded();
    applyServerDefaults(server);
    globalServerResetCallback = resetServerCallBack;
    globalServer = &server;
}