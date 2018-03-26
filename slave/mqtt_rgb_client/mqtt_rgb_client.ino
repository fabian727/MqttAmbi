/*
   Generic ESP8266 Modul
   1MB Flash
*/


#include <Adafruit_NeoPixel.h>
#include <MQTTClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#include "config.h"

/*
   Defines
*/

//for OTA
ESP8266HTTPUpdateServer httpUpdater;

//WS2812B Stripes
#define Leds 43
#define PinOne 0
#define PinTwo 2

//Wifi & MQTT
#ifndef WifiSsid
#define WifiSsid "WLAN_SSID"
#endif
#ifndef WifiPassword
#define WifiPassword "WLAN_PWD"
#endif

#ifndef MqttBroker
#define MqttBroker "Bro.ker.ipa.ddr"
#endif
#ifndef MqttPort
#define MqttPort 1883
#endif
#ifndef MqttName
#define MqttName "backlighter"
#endif

/*
   Global Variables
*/
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(Leds, PinOne, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(Leds, PinTwo, NEO_GRBW + NEO_KHZ800);

WiFiClient wifiClient;
MQTTClient mqttClient(256);

/*
   WebServer
*/
ESP8266WebServer httpServer(80);

static const char UpdatePage[] = "\
<html>\
  <body>\
  backlighter - v1.01 - Generic ESP8266-01 - Flash: 1 MB<br>\<br>\
    <form method='POST' action='/update' enctype='multipart/form-data'>\
      <input type='file' name='update'>\
      <input type='submit' value='Update'>\
    </form>\
  </body>\
</html>";

const char* update_username = "admin";
const char* update_password = "admin";

/*
   Mqtt Interrupt Message Receive Function
*/
void callback(MQTTClient *client, char topic[], char payload[], int length) {
  if (strcmp(topic, "desk/backlight/colour") == 0) {
    interprete_message(payload, length);
  } else if (strcmp(topic, "desk/backlight/numleds") == 0) {
    strip1.updateLength(payload[0]);
  } else if (strcmp(topic, "desk/backlight/numcolors") == 0) {

  }
}

void interprete_message(char payload[], int length) {
  uint8_t r = 0, g = 0, b = 0, w = 0;
  switch (payload[0]) {
    //setting only r,g,b
    case '1':
      //parse string-stream (payload) to int(r),int(g),int(b)
      for (int i = 2; i < length; i++) {
        b = b * 10;
        if (payload[i] != ',') {
          b += (payload[i] - '0');
        } else {
          r = g;
          g = b / 10;
          b = 0;
        }
      }
      b = b / 10;
      for (int i = 0; i < Leds; i++) {
        strip1.setPixelColor(i, r, g, b, 0);
        strip2.setPixelColor(i, r, g, b, 0);
      }
      break;
    //setting r,g,b,w, same principle as case '1', but for uint8_t
    case '2':
      //parse payload to int (r),int(g),int(b),int(w)
      for (int i = 2; i < length; i++) {
        w = w * 10;
        if (payload[i] != ',') {
          w += (payload[i] - '0');
        } else {
          b = g;
          g = r;
          r = w / 10;
          w = 0;
        }
      }
      w = w / 10;
      for (int i = 0; i < Leds; i++) {
        strip1.setPixelColor(i, b, g, r, w);
        strip2.setPixelColor(i, b, g, r, w);
      }
      break;
    case '3':
      r = payload[1];
      g = payload[2];
      b = payload[3];
      for (int i = 0; i < Leds; i++) {
        strip1.setPixelColor(i, r, g, b, 0);
        strip2.setPixelColor(i, r, g, b, 0);
      }
      break;
    case '4':
      r = payload[1];
      g = payload[2];
      b = payload[3];
      w = payload[4];
      for (int i = 0; i < Leds; i++) {
        strip1.setPixelColor(i, r, g, b, w);
        strip2.setPixelColor(i, r, g, b, w);
      }
      break;
    case '0':
//      strip1.setBuffer((uint8_t*) &payload[1], 0, length-1);
      strip2.setBuffer((uint8_t*) &payload[1], 0, length-1);
      break;
    case '9':
//      strip1.setRGBBuffer((uint8_t*) &payload[1], 0, length-1);
      strip2.setRGBBuffer((uint8_t*) &payload[1], 0, length-1);
      break;
    default:
      break;
  }

  strip1.show() ;
  strip2.show() ;
}

/*
   setup
*/

void setup() {

  //webserver
  MDNS.begin("backlighter");

  httpUpdater.setup(&httpServer, "/update", update_username, update_password);
  httpServer.on("/", HTTP_GET, [&]() {
    if (!httpServer.authenticate(update_username, update_password)) {
      httpServer.requestAuthentication();
      return;
    }
    httpServer.send(200, "text/html", UpdatePage);
  });

  httpServer.begin();

  MDNS.addService("http", "tcp", 80);

  //setup WS2812B Stripes
  for (int i = 0; i < Leds; i++) {
    strip1.setPixelColor(i, 0, 0, 0, 0);
    strip2.setPixelColor(i, 0, 0, 0, 0);
  }
  strip1.setPixelColor(0, 50, 0, 0, 0);
  strip1.show();
  strip1.begin();

  strip2.show();
  strip2.begin();

  //Setup Wifi && Mqtt
  WiFi.mode(WIFI_STA);
  WiFi.hostname(MqttName);
  WiFi.begin(WifiSsid, WifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  mqttClient.begin(MqttBroker, wifiClient);
  mqttClient.onMessageAdvanced(callback);

  while (!mqttClient.connect(MqttName)) {
    delay(1000); //1 second delay
  }

  mqttClient.subscribe("desk/backlight/colour", 0);
  mqttClient.subscribe("desk/backlight/numleds", 0);
  mqttClient.subscribe("desk/backlight/numcolors", 0);
  //Up and Running
  //  strip1.setPixelColor(0, 0, 50, 0, 0);
  //  strip1.show();
}

/*
   loop
*/
int activeLed = 0;
void loop() {
  /*
    delay(100);
    strip1.setPixelColor(activeLed-3,50,50,50,50);
    strip1.setPixelColor(activeLed-2,100,100,100,100);
    strip1.setPixelColor(activeLed-1,200,200,200,200);
    strip1.setPixelColor(activeLed,255,255,255,255);

    strip2.setPixelColor(activeLed-3,50,50,50,50);
    strip2.setPixelColor(activeLed-2,100,100,100,100);
    strip2.setPixelColor(activeLed-1,200,200,200,200);
    strip2.setPixelColor(activeLed,255,255,255,255);
    activeLed+=1;
    if(activeLed > Leds) {
      activeLed = 0;
    }
    strip1.show();
    strip2.show();
  */

  httpServer.handleClient();

  if (!mqttClient.connected()) {
    mqttClient.connect(MqttName);
  }
  mqttClient.loop();
}
