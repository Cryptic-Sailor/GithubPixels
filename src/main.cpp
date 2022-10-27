// External Libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
//#include <NeoPixelBus.h>
//#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// Global Variables
const uint8_t panelWidth = 32;
const uint8_t panelHeight = 8;
const uint16_t pixelCount = panelWidth * panelHeight;
const uint8_t brightness = 128;
bool clear = false;
bool github = false;
bool timer = false;
ESP8266WebServer server(80);

int cords[256][5];

uint8_t cordsCount = 0;

NeoTopology<ColumnMajorAlternatingLayout> topo(panelWidth, panelHeight);

NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(pixelCount);


// RgbColor green(0, 128, 0);
// RgbColor blue(0, 0, 128);
RgbColor black(0);

void handlePost()
{
  if (server.hasArg("plain") == false)
  {
    // handle error here
    server.send(200, "text/plain", "Body not received");
    return;
  }
  // Serial.println("POST");
  String body = server.arg("plain");
  Serial.println(body);
  DynamicJsonDocument doc(24700);
  doc.clear();
  DeserializationError error = deserializeJson(doc, body);
  if (error)
  {
    Serial.println("Error parsing JSON");
    server.send(400);
    return;
  }
  copyArray(doc["cords"], cords);
  cordsCount = (int)doc["count"];
  clear = (bool)doc["clear"];
  github = (bool)doc["github"];
  timer = (bool)doc["timer"];
  doc.clear();

  server.send(200, "application/json", "{ \"status\": 0 }");
}

void configureWebServer()
{
  server.on("/led", HTTP_POST, handlePost);
  // Add handler for switching modes here
}

void setup()
{
  WiFi.softAP("esp8266");
  Serial.begin(115200);

  server.begin();
  configureWebServer();

  strip.Begin();
  strip.Show();
  strip.ClearTo(black);
  strip.Show();
  strip.SetBrightness(brightness);
}

void loop()
{
  server.handleClient();

  if (clear)
  {
    strip.ClearTo(black);
    strip.Show();
    clear = false;
  }
  if (github)
  {
    // if (cordsCount > 257)
    // {
    //   cordsCount = 0;
    // }

    if (cordsCount > 0)
    {
      for (int i = 0; i < cordsCount; i++)
      {
        // Serial.print(cords[i][0]);
        // Serial.print(" ,");
        // Serial.println(cords[i][1]);
        // Serial.print(" ,");
        // Serial.println(cords[i][2]);
        // Serial.print(" ,");
        // Serial.println(cords[i][3]);
        // Serial.print(" ,");
        // Serial.println(cords[i][4]);

        strip.SetPixelColor(topo.Map(cords[i][0], cords[i][1]), RgbColor(cords[i][2], cords[i][3], cords[i][4]));
      }
      cordsCount = 0;
      strip.Show();
    }
     github = false;
  }
  if (timer){
    if (cordsCount > 0)
    {
      for (int i = 0; i < cordsCount; i++)
      {
        strip.SetPixelColor(topo.Map(cords[i][0], cords[i][1]), RgbColor(cords[i][2], cords[i][3], cords[i][4]));
        delay(1000);
        strip.Show();
      }
      cordsCount = 0;
    }
    timer = false;
  }
}
