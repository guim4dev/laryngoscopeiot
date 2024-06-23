#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void prepareWebserver(AsyncWebServer &server, std::function<void()> globalServerResetCallback);