// External Libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
//#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
//#include <ESPAsyncWebServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// Global Variables

const uint8_t panelWidth = 32;
const uint8_t panelHeight = 8;
const uint16_t pixelCount = panelWidth * panelHeight;
const uint8_t brightness = 128;
ESP8266WebServer server(80);


int cords[256][5];

uint8_t cordsCount = 0;

NeoTopology<ColumnMajorAlternatingLayout> topo(panelWidth, panelHeight);

NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(pixelCount);

// RgbColor green(0, 128, 0);
// RgbColor blue(0, 0, 128);
RgbColor black(0);

void handlePost(){
  if (server.hasArg("plain") == false) {
    //handle error here
    server.send(200, "text/plain", "Body not received");
    return;
  }
  //Serial.println("POST");
  String body = server.arg("plain");
  Serial.println(body);
  DynamicJsonDocument doc(24620);
  DeserializationError error = deserializeJson(doc, body);
  if (error)
  {
    Serial.println("Error parsing JSON");
    server.send(400);
    return;
  }
  copyArray(doc["cords"], cords);
  cordsCount = (int) doc["count"];


  server.send(200, "application/json", "{ \"status\": 0 }");
}

void configureWebServer()
{
  server.on("/led", HTTP_POST, handlePost);	

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
  server.handleClient();

  // if (cordsCount < 256)
  // {
  //   cordsCount = 0;
  // }
  strip.SetPixelColor(topo.Map(cords[cordsCount][0], cords[cordsCount][1]), RgbColor(cords[cordsCount][2], cords[cordsCount][3], cords[cordsCount][4]));
  cordsCount--;
  strip.Show();

  // for (int i = 0; i < cordsCount; i++)
  // {
  //   Serial.print(cords[i][0]);
  //   Serial.print(",");
  //   Serial.println(cords[i][1]);
  //   strip.SetPixelColor(topo.Map(cords[i][0], cords[i][1]), green);
  //   cordsCount--;

  //   strip.Show();
  // }

  // delay(1000);
}

