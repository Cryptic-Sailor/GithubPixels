// External Libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
//#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Global Variables

const uint8_t panelWidth = 32;
const uint8_t panelHeight = 8;
const uint16_t pixelCount = panelWidth * panelHeight;
const uint8_t brightness = 128;
AsyncWebServer server(80);

// coordinate buffer
uint16_t coordBuffer[pixelCount];

int cords[256][2];
uint8_t cordsCount = 0;

NeoTopology<ColumnMajorAlternatingLayout> topo(panelWidth, panelHeight);

NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(pixelCount);

RgbColor green(0, 128, 0);
RgbColor blue(0, 0, 128);
RgbColor black(0);

void configureWebServer()
{
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        if ((request->url() == "/post-message") && (request->method() == HTTP_POST))
        {
          DynamicJsonDocument doc(512);
          Serial.println((const char *) data);
          DeserializationError error = deserializeJson(doc, (const char *) data);
          if (error)
          {
            Serial.println("Error parsing JSON");
            request->send(400);
            return;
          }
          copyArray(doc["cords"], cords);
          cordsCount = (int) doc["count"];

          request->send(200, "application/json", "{ \"status\": 0 }");
        } 
  });

}

void setup()
{
  WiFi.softAP("esp8266");
  Serial.begin(115200);
  server.begin();
  configureWebServer();
  strip.Begin();
  strip.Show();

  delay(1000);
  strip.ClearTo(black);
  strip.Show();

  delay(1000);

  strip.SetBrightness(brightness);

}

void loop()
{
  if (cordsCount < 256)
  {
    cordsCount = 0;
  }

  for (int i = 0; i < cordsCount; i++) if (cordsCount > 0)
  {
    Serial.print(cords[i][0]);
    Serial.print(",");
    Serial.println(cords[i][1]);
    strip.SetPixelColor(topo.Map(cords[i][0], cords[i][1]), green);
    cordsCount--;

    strip.Show();
  }

  // delay(1000);
}

