#include "MqttClient.h"
#include <string.h>

#include "type.h"
#include <sys/types.h>

#include <vector>

using namespace std;

MqttClient::MqttClient(const char *id, const char *host, int port) : mosquittopp(id) {
	mosqpp::lib_init();			// Initialize libmosquitto

	this->NumColours = 4;
	this->NumLeds = 64;

	this->MainTopic = "/desk/backlight";
	int keepalive = 120; // seconds
	connect(host, port, keepalive);		// Connect to MQTT Broker
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
	int color = 0;
#define MinOffset 0
#define MaxOffset 255
	for(int leds=0;leds < NumLeds;leds++) {
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
		mqttStream[color++] = 0; //white (warm and/or cold)
	}
}

void MqttClient::confNumLeds() {
	string feedback;
 	vector <uint8_t> mqttStream;
 	int NumLeds = 1;
	for(int i=0;i<this->NumColours;i++) {
		mqttStream.insert(mqttStream.begin(),255);
	}

	string topic = this->MainTopic + "/colours";
	this->publish(NULL,&topic[0],(this->NumLeds*this->NumColours),&mqttStream[0]);
	printf("-------------------------\n");
	printf("How much LEDs do you use?\n");
	printf("+ for more LEDs\n");
	printf("- for less LEDs\n");
	printf("number of LEDs\n");
	printf("y, if last LED is shining\n");
	printf("-------------------------\n");

	int MaxNumLeds = NumLeds;
	while(feedback[0] != 'y') {
		scanf("%s",&feedback[0]);
		if(feedback[0] == '+') {
			this->NumLeds++;
			MaxNumLeds++;
			for(int i=0;i<NumColours;i++) {
				mqttStream.insert(mqttStream.begin(),0);
			}
		} else if(feedback[0] == '-'){
			this->NumLeds--;
			mqttStream.erase(mqttStream.begin(), mqttStream.begin() + 4);
			for(int i=0;i<NumColours;i++) {
				mqttStream.insert(mqttStream.end(),0);
			}
		} else if(feedback[0] >= '0' && feedback[0] <= '9' ) {
			int i = 0;
			int num=0;
			while(feedback[i] != '\n') {
				num = num*10 + feedback[i]-'0';
			}
			if(num > 0) {
				this->NumLeds = num;
			}
			if(num > MaxNumLeds) {
				MaxNumLeds = num;
			}
		}
		if(this->NumLeds < 1) {
			this->NumLeds = 1;
		}
		this->publish(NULL,&topic[0],(MaxNumLeds*this->NumColours),&mqttStream[0]);
	}
	printf("got %i LEDs\n",NumLeds);
}
