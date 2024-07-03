#include <Arduino.h>
#include <WiFi.h>

void setupNetwork()
{
    // Get the MAC address of the Wi-Fi AP interface
    String macAddress = WiFi.macAddress();
    String ssid = "laringoscopiot-" + macAddress;
    String password = "laringoscopiot";

    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
}
