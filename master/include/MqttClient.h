#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include <QThread> //QT needs to be first

#include <mosquitto.h> //c-header
#include <mosquittopp.h> //cpp-header equivalent
#include <iomanip> //uint8_t
#include <string> //string

#include "ws2812b.h"

using namespace std;

class MqttClient : public mosqpp::mosquittopp, public QThread
{
public:
    MqttClient(const char *id, const char *host, const char *maintopic, int port);
	~MqttClient();

    string MainTopic;
    void on_connect(int rc);
	void on_subcribe(int mid, int qos_count, const int *granted_qos);
    void createByteStream(int NumLeds, int NumColors, uint8_t *mqttStream, QColor *averageColor);
    void createStringStream(int NumLeds,int NumColors,char *mqttStream, QColor *averageColor);

private:
    void run();
    void *msg_buffer;
    ws2812b *stripe;
};

#endif // _MQTTCLIENT_H_
