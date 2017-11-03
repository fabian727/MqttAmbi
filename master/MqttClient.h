#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include <mosquitto.h> //c-header
#include <mosquittopp.h> //cpp-header equivalent
#include <iomanip> //uint8_t
#include "type.h" //Color
#include <string> //string

using namespace std;

class MqttClient : public mosqpp::mosquittopp
{
	int NumLeds;
	int NumColours;
	string MainTopic;
public:
	MqttClient(const char *id, const char *host, int port);
	~MqttClient();

	void on_connect(int rc);
	void on_subcribe(int mid, int qos_count, const int *granted_qos);
	void createByteStream(int NumLeds,int NumColors,uint8_t *mqttStream, Color *averageColor);

	void confNumLeds(void);
};

#endif // _MQTTCLIENT_H_
