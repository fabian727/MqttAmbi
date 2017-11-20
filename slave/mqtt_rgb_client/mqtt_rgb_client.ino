#include <Adafruit_NeoPixel.h>
#include <MQTTClient.h>
#include <ESP8266WiFi.h>

#include "config.h"

//for serial feedback
#undef DEBUG
  
//WS2812B Stripes
#define Leds 32
#define PinOne 2

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
 * Global Variables
 */
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(Leds, PinOne,NEO_GRBW + NEO_KHZ800);

WiFiClient wifiClient;
MQTTClient mqttClient(256);

/*
 * Mqtt Interrupt Message Receive Function
 */
void callback(MQTTClient *client, char topic[], char payload[], int length) {
#ifdef DEBUG
  Serial.print("Mqtt message incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(length);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
#endif
  if(strcmp(topic,"desk/backlight/colour") == 0) {
    #ifdef DEBUG
    Serial.println("got correct topic");
    #endif
    strip1.setBuffer((uint8_t*) &payload[0],0,length);
    strip1.show() ;
  }
}

/*
 * setup
 */
void setup() {

  //setup WS2812B Stripes
  for(int i=0;i<Leds;i++) {
    strip1.setPixelColor(i,0,0,0,0);
  }
  strip1.setPixelColor(0,0,0,0,50);
  strip1.show();

#ifdef DEBUG
  //Serial Setup
  Serial.begin(115200);
#endif
  strip1.begin();

  //Setup Wifi && Mqtt
  WiFi.hostname("backlighter");
  WiFi.begin(WifiSsid, WifiPassword);
#ifdef DEBUG
  Serial.println("");
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG
    Serial.print(".");

#endif
  }
  WiFi.mode(WIFI_STA);
#ifdef DEBUG
  Serial.println();
  Serial.println(WiFi.localIP());
#endif
  mqttClient.begin("192.168.2.12", wifiClient);
  mqttClient.onMessageAdvanced(callback);

  while (!mqttClient.connect("backlighter")) {
    delay(1000); //1 second delay
#ifdef DEBUG
    Serial.print(".");
#endif
  }
  mqttClient.subscribe("desk/backlight/colour",0);
#ifdef DEBUG
  Serial.println();
  Serial.println("Mqtt broker connected and topics subscribed");
#endif
  //Up and Running
  strip1.setPixelColor(0,0,50,0,0);
  strip1.show();
}

int numloop=0;

/*
 * loop
 */
void loop() {
  while(!mqttClient.connected()) {
    mqttClient.connect(MqttName);
    for(int i=0;i<Leds;i++) {
      strip1.setPixelColor(i,0,0,0,0);
    }
    strip1.setPixelColor(0,50,0,0,0);
    strip1.show();
    delay(1);
  }
  mqttClient.loop();
#ifdef DEBUG
  Serial.print(".");
  if(numloop == 10) {
    numloop = 0;
    Serial.println();
  }
  numloop++;
#endif
}

/*
 * theaterChase
 */
void theaterChase(uint32_t c, uint8_t wait) {
    for (int j=0; j<10; j++) {  //do 10 cycles of chasing
        for (int q=0; q < 3; q++) {
            for (int i=0; i < strip1.numPixels(); i=i+3) {
                strip1.setPixelColor(i+q, c);    //turn every third pixel on
            }
            strip1.show();

            delay(wait);

            for (int i=0; i < strip1.numPixels(); i=i+3) {
                strip1.setPixelColor(i+q, 0);        //turn every third pixel off
            }
        }
    }
}
