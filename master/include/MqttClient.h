#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include <QThread> //QT needs to be first

#include <mosquitto.h> //c-header
#include <mosquittopp.h> //cpp-header equivalent
#include <iomanip> //uint8_t
#include <string> //string

#include "ws2812b.h"

using namespace std;

#define MQTT_TOPIC_COLOUR this->topic + "/colour"
#define MQTT_TOPIC_NUMLEDS this->topic + "/numleds"

class MqttClient : public QThread, public mosqpp::mosquittopp
{
    Q_OBJECT
public:
    explicit MqttClient(const char *id, const char *host, const char *topic, int port);
	~MqttClient();

    string topic;
    void createByteStream(int NumLeds, int NumColors, uint8_t *mqttStream, QColor *averageColor);
    void createStringStream(int NumLeds,int NumColors,char *mqttStream, QColor *averageColor);
    void maxBrightness();
private:
    void run();
    bool running;
    void CalcAverageColor(uint8_t *buffer, XImage *image, int xOffset, int yOffset, int width, int height);
    int colors;
    uint8_t leds;

public slots:
    void getColour(QColor avgcolour);
    void getAmbi(bool checked);
    void getNumLeds(int leds);
    void getTopic(std::string maintopic);
    void getStop();
};

/*
 * decoding scheme for the client
 * SingleLed: each Byte will be sent directly to the LED stripe
 * RGBColor: only a RGB colored stripe is attached
 * RGBWColor: a RGBW colored stripe is attached
 */

#define BYTE_WISE '0'       //take array as is and copy paste it
#define BYTE_RGB  '9'       //not complete as is, augment it with 0 for white
#define RGB_CHAR  '1'
#define RGBW_CHAR '2'
#define RGB_INT   '3'
#define RGBW_INT  '4'

#endif // _MQTTCLIENT_H_
