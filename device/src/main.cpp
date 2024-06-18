#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
// Libs Used for Camera Streaming V1
#include "esp_camera.h"
#include "soc/soc.h"          //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define CAMERA_MODEL_AI_THINKER

// Replace with your network credentials
const char *ssid = "laringoscopiot";
const char *password = "senhasecreta";

#define LED_BUILTIN 4
#define TEETH_BUTTON_PIN 2
#define PRIMARY_FORCE_SENSOR_PIN 12
#define SECONDARY_FORCE_SENSOR_PIN 13

#define PART_BOUNDARY "123456789000000000000987654321"

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

static const char *JPG_CONTENT_TYPE = "image/jpeg";

void setupTeethButton()
{
    pinMode(TEETH_BUTTON_PIN, INPUT_PULLUP); // Button is connected to GND
}

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

void setupWifi()
{
    // // Configures static IP address
    // IPAddress local_IP(192, 168, 0, 222);
    // IPAddress gateway(192, 168, 0, 1);
    // IPAddress subnet(255, 255, 255, 0);
    // while (!WiFi.config(local_IP, gateway, subnet)) {
    //   Serial.println("STA Failed to configure");
    //   digitalWrite(LED_BUILTIN, HIGH);
    //   delay(2000);
    //   digitalWrite(LED_BUILTIN, LOW);
    //   Serial.print(".");
    // }

    // Wi-Fi connection
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(2000);
        digitalWrite(LED_BUILTIN, LOW);
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

void blinkOnce()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
}

void blinkThrice()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
}

int teethSensor = 0;
void captureTeethSensorValue()
{
    teethSensor = !digitalRead(TEETH_BUTTON_PIN); // button is connected to GND - so when pressed, it will read LOW.
}

struct ForceSensorsCapture
{
    int primaryForce;
    int secondaryForce;
} sensorsCapture;

void captureForceSensorsValues()
{
    // int primaryForceValue = analogRead(PRIMARY_FORCE_SENSOR_PIN);
    // int secondaryForceValue = analogRead(SECONDARY_FORCE_SENSOR_PIN);
    // Serial.print("Secondary force sensor value: ");
    // Serial.println(secondaryForceValue);
    // TMP: get random value between 0 and 4095
    int primaryForceValue = random(4095);
    int secondaryForceValue = random(4095);
    // Serial.print("Primary force sensor value: ");
    // Serial.println(primaryForceValue);

    sensorsCapture.primaryForce = primaryForceValue;
    sensorsCapture.secondaryForce = secondaryForceValue;
}

// TODO: maybe remove this
String processor(const String &var)
{
    return String();
}

AsyncWebServer server(80);
AsyncEventSource sensorEvents("/sensors");
AsyncEventSource cameraUpdates("/camera");

const char alive_json[] PROGMEM = R"rawliteral(
{
  "alive": true
})rawliteral";

unsigned long sensors_delay = 1000; // 1 second delay
bool sensorsTaskStarted = false;
void sensorsTask(void *z)
{
    sensorsTaskStarted = true;
    while (1)
    {
        delay(sensors_delay);
        captureTeethSensorValue();
        captureForceSensorsValues();
        String sensorData = "{\"primaryForce\": " + String(sensorsCapture.primaryForce) + ", \"secondaryForce\": " + String(sensorsCapture.secondaryForce) + ", \"teethPressed\": " + String(teethSensor) + " }";
        sensorEvents.send(sensorData.c_str(), "sensorData", millis());
    }
}

bool cameraTaskStarted = false;
// unsigned long camera_delay = 41.6; // 24fps
unsigned long camera_delay = 500; // DEBUG: 2fps
void cameraTask(void *z)
{
    int errorCounter = 0;
    cameraTaskStarted = true;
    while (1)
    {
        if (errorCounter > 5)
        {
            Serial.println("Too many camera errors. Stopping camera task.");
            cameraUpdates.send("Too many camera errors. Stopping camera task.", "cameraFail", millis());
            cameraTaskStarted = false;
            vTaskDelete(NULL);
        };
        delay(camera_delay);
        {
            camera_fb_t *fb = esp_camera_fb_get();
            if (fb == NULL)
            {
                Serial.println("Camera frame failed");
                cameraUpdates.send("cameraFrameFailed", "cameraFail", millis());
                errorCounter++;
                continue;
            }

            char *sendable_buffer = NULL;
            size_t jpg_buf_len = 0;
            // already a jpg
            if (fb->format == PIXFORMAT_JPEG)
            {
                Serial.println("No need to convert to jpg");
                jpg_buf_len = fb->len;
                sendable_buffer = (char *)malloc(jpg_buf_len + 1);
            }
            else // need to convert to jpg
            {
                Serial.println("Converting to jpg...");
                uint8_t *jpg_buf = NULL;
                bool jpeg_converted = frame2jpg(fb, 80, &jpg_buf, &jpg_buf_len);
                esp_camera_fb_return(fb);
                if (!jpeg_converted)
                {
                    Serial.println("JPEG compression failed");
                    cameraUpdates.send("Failed to convert to JPG", "cameraFail", millis());
                    errorCounter++;
                    continue;
                }
                sendable_buffer = (char *)malloc(jpg_buf_len + 1);
                Serial.println("Converted to jpg.");
            }
            Serial.println("JPEG: " + String(jpg_buf_len));
            errorCounter = 0;
            cameraUpdates.send(sendable_buffer, "cameraFrame", millis());
        }
    }
}

void setupAsyncWebServer()
{
    // setup cors and OPTIONS support
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else {
      request->send(404);
    } });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", alive_json, processor); });

    sensorEvents.onConnect([](AsyncEventSourceClient *client)
                           {
        if (client->lastId())
        {
            Serial.printf("Client reconnected to sensorEvents! Last message ID that it got is: %u\n", client->lastId());
        }

        client->send("connected", NULL, millis(), 10000);

        if (!sensorsTaskStarted)
        {
            xTaskCreatePinnedToCore(sensorsTask, "sensorsTask", 4096, NULL, 10, NULL, 0);
        }; });

    cameraUpdates.onConnect([](AsyncEventSourceClient *client)
                            {
                                if (client->lastId())
                                {
                                    Serial.printf("Client reconnected to cameraUpdates! Last message ID that it got is: %u\n", client->lastId());
                                }

                                client->send("connected", NULL, millis(), 10000);
                                if (!cameraTaskStarted)
                                {
                                    xTaskCreatePinnedToCore(cameraTask, "cameraTask", 4096, NULL, 10, NULL, 0);
                                }; });

    server.on("/camera/status", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        sensor_t * s = esp_camera_sensor_get();
        if(s == NULL){
            request->send(501);
            return;
        }

        request->send(200, "application/json", String("{\"status\": \"up\", \"taskStarted\":" + String(cameraTaskStarted) + " }") ); });

    server.addHandler(&sensorEvents);
    server.addHandler(&cameraUpdates);
    server.begin();
}

void setupCamera()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound())
    {
        Serial.println("PSRAM found");
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 20;
        config.fb_count = 1;
    }
    else
    {
        Serial.println("PSRAM not found");
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 20;
        config.fb_count = 1;
    }

    // Camera init
    esp_err_t err = esp_camera_init(&config);
    Serial.println("CAMERA INIT ERR: " + String(err));
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    Serial.println("Camera setup done successfully");
    Serial.flush();
}

// CPU loggers
long ctr0 = 0;
void id0(void *z)
{
    while (1)
    {
        ctr0++;
        delay(10);
    }
}

long ctr1 = 0;
void id1(void *z)
{
    while (1)
    {
        ctr1++;
        delay(10);
    }
}

void ctrLoggerTask(void *z)
{
    while (1)
    {
        Serial.println("CPU 0 CTR: " + String(ctr0));
        Serial.println("CPU 1 CTR: " + String(ctr1));
        Serial.flush();
        delay(3000); // every 3 seconds
    }
}

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector

    Serial.begin(115200);
    Serial.setDebugOutput(true);
    pinMode(LED_BUILTIN, OUTPUT);

    blinkThrice();
    Serial.println("Setup starting...");
    // TASKS FOR CPU USAGE LOGGING
    // xTaskCreatePinnedToCore(id0, "id0", 4096, NULL, 0, NULL, 0);
    // xTaskCreatePinnedToCore(id1, "id1", 4096, NULL, 0, NULL, 1);
    // xTaskCreate(ctrLoggerTask, "ctrLoggerTask", 4096, NULL, 0, NULL);
    Serial.println("Initial Free PSRAM: " + String(ESP.getFreePsram()));
    setupWifi();
    setupTeethButton();
    Serial.println("Free PSRAM before camera: " + String(ESP.getFreePsram()));
    setupCamera();
    Serial.println("Free PSRAM after camera: " + String(ESP.getFreePsram()));
    setupAsyncWebServer();
    Serial.println("Free PSRAM after async webServer: " + String(ESP.getFreePsram()));
    Serial.println("Setup done");
    Serial.println("Total heap: " + String(ESP.getHeapSize()));
    Serial.println("Free heap: " + String(ESP.getFreeHeap()));
    Serial.println("Total PSRAM: " + String(ESP.getPsramSize()));
    Serial.println("Free PSRAM: " + String(ESP.getFreePsram()));
    Serial.flush();
}

// What todo with the loop ?
// TODO: move sensors to separate file + use RTOsTasks
void loop() {}