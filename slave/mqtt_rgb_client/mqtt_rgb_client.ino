#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

//for serial feedback
#define DEBUG


//WS2812B Stripes
#define Leds 32
#define PinOne 2

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(Leds, PinOne,
NEO_GRBW + NEO_KHZ800);

//Wifi & MQTT
#define WifiSsid "My_SSID"
#define WifiPassword "My_PWD"

#define MqttServer "Server.IP"
#define MqttPort 1883
#define MqttName "backlighter"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

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
  strip1.begin();
#endif

  //Setup Wifi && Mqtt
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
  mqttClient.setServer(MqttServer, 1883);
  mqttClient.setCallback(callback);
  while (!mqttClient.connected()) {
    mqttClient.connect(MqttName);
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

// the loop function runs over and over again forever
void loop() {
  if (!mqttClient.connected()) {
    mqttClient.connect(MqttName);
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

//Mqtt On-Message-Receive called function
void callback(char* topic, byte * payload, unsigned int length) {
#ifdef DEBUG
  Serial.print("Mqtt message incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(length);
  Serial.print(" - ");
  Serial.print((char*) payload);
  Serial.println();
#endif
  if(strcmp(topic,"desk/backlight/colour") == 0) {
    Serial.println("got correct topic");
    for(int clr=0,led = 0; clr < length;led++) {
      strip1.setPixelColor(led,payload[clr],payload[clr+1],payload[clr+2],payload[clr+3]);
      clr += 4;
#ifdef DEBUG
      Serial.print("setting led: ");
      Serial.println(led);
#endif
    }
    strip1.show();
  }
}




