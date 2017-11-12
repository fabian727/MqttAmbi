#include "MqttClient.h"
#include <string.h>
#include <vector>
#include <sstream>

#include "type.h"
#include <sys/types.h>

using namespace std;

/* ToDo: SetUp last Will, all LEDS go black */

MqttClient::MqttClient(const char *id, const char *host, const char *maintopic, int port) : mosquittopp(id) {
	mosqpp::lib_init();			// Initialize libmosquitto

    this->MainTopic = maintopic;
	int keepalive = 120; // seconds
	connect(host, port, keepalive);		// Connect to MQTT Broker

/*set last will*/
//	this->will_set(&topic[0],NumLeds*NumColors,&mqttStream[0]);

}

MqttClient::~MqttClient() {
	this->disconnect();
}

void MqttClient::on_connect(int rc) {
//	printf("Connected with code %d. \n", rc);
}

void MqttClient::on_subcribe(int mid, int qos_count, const int *granted_qos) {
//	printf("Subscription succeeded. \n");
}

void MqttClient::createByteStream(int NumLeds,int NumColors,uint8_t *mqttStream, Color *averageColor) {
#define MinOffset 0
#define MaxOffset 255
    for(int color=0, leds=0;leds < NumLeds;leds++) {
		if(averageColor[leds].green < MinOffset) {
			mqttStream[color++] = 0;
		}else if (averageColor[leds].green > MaxOffset) {
			mqttStream[color++] = 255;
		}else {
			mqttStream[color++] = averageColor[leds].green;
		}
		if(averageColor[leds].red < MinOffset) {
			mqttStream[color++] = 0;
		}else if (averageColor[leds].red > MaxOffset) {
			mqttStream[color++] = 255;
		}else {
			mqttStream[color++] = averageColor[leds].red;
		}
		if(averageColor[leds].blue < MinOffset) {
			mqttStream[color++] = 0;
		}else if (averageColor[leds].blue > MaxOffset) {
			mqttStream[color++] = 255;
		}else {
			mqttStream[color++] = averageColor[leds].blue;
		}
        if(NumColors >= 4) {
            mqttStream[color++] = 0; //white (warm and/or cold)
        }
	}
}

void MqttClient::createStringStream(int NumLeds,int NumColors,char *mqttStream, Color *averageColor) {
    stringstream stream;

    stream.str("");
    for(int leds=0;leds<NumLeds;leds++) {
        stream << std::setw(sizeof('T')*2)
               << std::hex << averageColor[leds].green;
        stream << std::setfill ('0') << std::setw(sizeof('T')*2)
               << std::hex << averageColor[leds].red;
        stream << std::setfill ('0') << std::setw(sizeof('T')*2)
               << std::hex << averageColor[leds].blue;
        if(NumColors >= 4) {
        stream << "00";
        }
    }
    int i =0;
    while(stream.str()[i] != '\n' || stream.str()[i] != '\r') {
        mqttStream[i] = stream.str()[i];
    }
}
