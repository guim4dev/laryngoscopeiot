#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
// Libs Used for Camera Streaming V1
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "soc/soc.h"          //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems

// Libs for async web server
// namespace async_server {
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
// }

#define CAMERA_MODEL_AI_THINKER

// Replace with your network credentials
const char *ssid = "laringoscopiot";
const char *password = "senhadopai";

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

// TODO: current implementation sends the whole image every time. It would be better to send only the changed parts.
// we should also consider sending a lower resolution image to save bandwidth
// and using HLS streaming for better performance on the client side
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";
static AsyncWebServerRequest *cameraRequest = NULL;

static void camera_stream_handler(void * parameter)
{
  if (!cameraRequest) {
    return;
  }

  camera_fb_t *fb = NULL;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char *part_buf[64];

  fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    cameraRequest->send(500, "text/plain", "Camera capture failed");
    return;
  }
  else
  {
    if (fb->width > 400)
    {
      if (fb->format != PIXFORMAT_JPEG)
      {
        bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted)
        {
          Serial.println("JPEG compression failed");
          cameraRequest->send(500, "text/plain", "Camera capture failed");
        }
      }
      else
      {
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;
      }
    }
  }
  Serial.println("JPG buffer length: " + String(_jpg_buf_len) + " bytes");
  AsyncWebServerResponse *response = cameraRequest->beginResponse_P(200, "image/jpeg", _jpg_buf, _jpg_buf_len);
  cameraRequest->send(response);
  cameraRequest = NULL;
  Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));

  // delete task
  vTaskDelete(NULL);

  // while (true)
  // {
  //   Serial.print("A tarefa está rodando no seguinte núcleo: ");
  //   Serial.println(xPortGetCoreID());
  //   fb = esp_camera_fb_get();
  //   if (!fb)
  //   {
  //     Serial.println("Camera capture failed");
  //     res = ESP_FAIL;
  //   }
  //   else
  //   {
  //     if (fb->width > 400)
  //     {
  //       if (fb->format != PIXFORMAT_JPEG)
  //       {
  //         bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
  //         esp_camera_fb_return(fb);
  //         fb = NULL;
  //         if (!jpeg_converted)
  //         {
  //           Serial.println("JPEG compression failed");
  //           res = ESP_FAIL;
  //         }
  //       }
  //       else
  //       {
  //         _jpg_buf_len = fb->len;
  //         _jpg_buf = fb->buf;
  //       }
  //     }
  //   }
  //   Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
  // }
  // return res;
}

// void startCameraServer(void * parameter)
// {
//   esp_http::httpd_config_t config = HTTPD_DEFAULT_CONFIG();
//   config.server_port = 81;

//   esp_http::httpd_uri_t index_uri = {
//       .uri = "/camera",
//       .method = esp_http::HTTP_GET,
//       .handler = camera_stream_handler,
//       .user_ctx = NULL};

//   Serial.printf("Starting web server on port: '%d'\n", config.server_port);
//   if (httpd_start(&stream_httpd, &config) == ESP_OK)
//   {
//     httpd_register_uri_handler(stream_httpd, &index_uri);
//   }
// }

bool cameraSuccesfullyInitialized = false;

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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
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
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // xTaskCreatePinnedToCore(
  //     startCameraServer, /* Function to implement the task */
  //     "camera_server",  /* Name of the task */
  //     20000,            /* Stack size in words */
  //     NULL,             /* Task input parameter */
  //     0,                /* Priority of the task */
  //     NULL,             /* Task handle. */
  //     1                 /* Core where the task should run */
  // );
  cameraSuccesfullyInitialized = true;
}

void setupTeethButton()
{
  pinMode(TEETH_BUTTON_PIN, INPUT_PULLUP); // Button is connected to GND
}

String discordWebhookUrl = "http://eoovsvr9ws6bcrm.m.pipedream.net";

void notifyDiscordWebhook(String content)
{
  HTTPClient http;
  WiFiClient client;

  http.begin(client, discordWebhookUrl.c_str());
  http.addHeader("Content-Type", "application/json");

  String json = "{\"content\":\"" + content + "\"}";
  int httpCode = http.POST(json);
  http.end();

  Serial.println("Notified Discord webhook with content: " + content);
  Serial.println("Discord webhook response: " + httpCode);
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

void blinkOnce() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
}

void blinkThrice() {
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

struct ForceSensorsCapture {
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
String processor(const String& var) {
  return String();
}

AsyncWebServer server(80);
AsyncEventSource sensorEvents("/sensors");

const char alive_json[] PROGMEM = R"rawliteral(
{
  "alive": true
})rawliteral";

unsigned long last_time_sensors = 0;
unsigned long sensors_delay = 500; // 0.5 seconds delay

bool connectedToSensors = false;
bool connectedToCamera = false;

void setupAsyncWebServer () {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "application/json", alive_json, processor);
  });

  // TODO: fix camera
  // server.on("/camera", HTTP_GET, [](AsyncWebServerRequest *request){
  //   cameraRequest = request;
  //   xTaskCreate(
  //     camera_stream_handler,  
  //     "camera_stream_task",
  //     10000,
  //     NULL,
  //     1,
  //     NULL
  //   );
  // });

  // setup cors and OPTIONS support
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.onNotFound([](AsyncWebServerRequest *request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else {
      request->send(404);
    }
  });

  sensorEvents.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected to sensorEvents! Last message ID that it got is: %u\n", client->lastId());
    }

    client->send("connected", NULL, millis(), 10000);
    connectedToSensors = true;
  });

  server.addHandler(&sensorEvents);
  server.begin();
}

void setup()
{ 
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  pinMode(LED_BUILTIN, OUTPUT);

  blinkThrice();
  Serial.println("Setup starting...");
  
  setupWifi();
  setupTeethButton();
  // TODO: FIX CAMERA
  // setupCamera();
  setupAsyncWebServer();
  Serial.println("Setup done");
  Serial.flush();
}

// TODO: move sensors to separate file + use RTOsTasks
void loop()
{
  int loop_start = millis();
  int time_diff_sensors = loop_start - last_time_sensors;
  if (time_diff_sensors > sensors_delay && connectedToSensors) {
    captureTeethSensorValue();
    captureForceSensorsValues();
    String sensorData = "{\"primaryForce\": " + String(sensorsCapture.primaryForce) + ", \"secondaryForce\": " + String(sensorsCapture.secondaryForce) + ", \"teethPressed\": " + String(teethSensor) + " }";
    sensorEvents.send(sensorData.c_str(), "sensorData", millis());
    Serial.flush();
    last_time_sensors = loop_start;
  }
}
