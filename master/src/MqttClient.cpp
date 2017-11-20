#include <QColor>

#include "MqttClient.h"
#include <string.h>
#include <vector>
#include <sstream>

#include <sys/types.h>

#include "config.h"
#include "ws2812b.h"

#ifdef LinuxX11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#include <iostream>
using namespace std;

typedef int (*handler)(Display *, XErrorEvent *);
int handleX11Error(Display *display, XErrorEvent *error) {
    printf("X11 Error: %i \n", error->error_code);
   return error->error_code;
}




/* ToDo: SetUp last Will, all LEDS go black */

MqttClient::MqttClient(const char *id, const char *host, const char *maintopic, int port) : mosquittopp(id), QThread() {
    mosqpp::lib_init();			// Initialize libmosquitto

    /* setup ws2812b */
    this->stripe = new ws2812b(32,4);

    msg_buffer = new uint8_t[stripe->getNumColors()*stripe->getNumLeds()];
    memset(msg_buffer,0,32*4);

    this->MainTopic = maintopic;
    int keepalive = 120; // seconds
    mosquittopp::connect(host, port, keepalive);		// Connect to MQTT Broker

/*set last will*/
    this->will_set("desk/backlight/color",stripe->getNumLeds()*stripe->getNumColors(),msg_buffer);
}

MqttClient::~MqttClient() {
    this->mosquittopp::disconnect();
}

void MqttClient::run() {

#ifdef MQTT_STRING
    string mqttStream;
#else
    int ld=stripe->getNumLeds();
    int clr=stripe->getNumColors();
    int mix=clr*ld;
    uint8_t mqttStream[mix];
#endif

    /* setup x11 */
    Display *display = XOpenDisplay(NULL);
    Screen  *screen  = DefaultScreenOfDisplay(display);
    XImage *image;
    XSetErrorHandler(&handleX11Error);
    QColor averageColor[stripe->getNumLeds()];


    while(true) {
#ifdef TIME
    auto start = std::chrono::high_resolution_clock::now();
#endif
       image = XGetImage(display,RootWindow (display, DefaultScreen (display)), 0,0 , screen->width,screen->height,AllPlanes, ZPixmap);
       stripe->CalcAverageColor(&averageColor[0],image,screen->width,20,screen->width,screen->height-20);

#ifdef MQTT_STRING
/* string transfer, every 2 bytes/chars equals 1 hex number/color */
    mqtt->createStringStream(NumLeds,NumColors,&mqttStream[0],&averageColor[0]);
       mqtt->publish(NULL,"/desk/backlight" + "/colour",NumLeds*NumColors*2,&mqttStream[0]);
#else
/* binary transfer, every byte/char equals 1 hex number/color */
    this->createByteStream(stripe->getNumLeds(),stripe->getNumColors(),&mqttStream[0],&averageColor[0]);

    string subtopic = this->MainTopic + "/colour";
//    this->publish(NULL,&subtopic[0],(stripe->leds*stripe->colors),&mqttStream[0]);
    this->publish(NULL,&subtopic[0],100,&mqttStream[0]);

#endif //MQTT_STRING
    if(this->loop()) {
        this->reconnect();
    }

#ifdef TIME
    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);
    std::cout << "xGetImage: " << microseconds.count() << " µs\n";
#endif //TIME

#ifdef DEBUG
        draw_led_grid(averageColor,stripe->leds,1);
        draw_grid(image);
#endif //DEBUG

        XDestroyImage(image);
    }
}

void MqttClient::on_connect(int rc) {
//	printf("Connected with code %d. \n", rc);
}

    void MqttClient::on_subcribe(int mid, int qos_count, const int *granted_qos) {
//	printf("Subscription succeeded. \n");
}

void MqttClient::createByteStream(int NumLeds,int NumColors,uint8_t *mqttStream, QColor *averageColor) {
#define MinOffset 0
#define MaxOffset 255
    for(int color=0, leds=0;leds < NumLeds;leds++) {
        if(averageColor[leds].green() < MinOffset) {
            mqttStream[color++] = 0;
        }else if (averageColor[leds].green() > MaxOffset) {
            mqttStream[color++] = 255;
        }else {
            mqttStream[color++] = averageColor[leds].green();
        }
        if(averageColor[leds].red() < MinOffset) {
            mqttStream[color++] = 0;
        }else if (averageColor[leds].red() > MaxOffset) {
            mqttStream[color++] = 255;
        }else {
            mqttStream[color++] = averageColor[leds].red();
        }
        if(averageColor[leds].blue() < MinOffset) {
            mqttStream[color++] = 0;
        }else if (averageColor[leds].blue() > MaxOffset) {
            mqttStream[color++] = 255;
        }else {
            mqttStream[color++] = averageColor[leds].blue();
        }
        if(NumColors >= 4) {
            mqttStream[color++] = 0; //1. white (warm and/or cold)
        }
        if(NumColors >= 5) {
            mqttStream[color++] = 0; //2. white (warm and/or cold)
        }
    }
}

void MqttClient::createStringStream(int NumLeds,int NumColors,char *mqttStream, QColor *averageColor) {
    stringstream stream;

    stream.str("");
    for(int leds=0;leds<NumLeds;leds++) {
        stream << std::setfill ('0') << std::setw(sizeof('T')*2)
               << std::hex << averageColor[leds].red();
        stream << std::setfill ('0') << std::setw(sizeof('T')*2)
               << std::hex << averageColor[leds].green();
        stream << std::setfill ('0') << std::setw(sizeof('T')*2)
               << std::hex << averageColor[leds].blue();
        if(NumColors >= 4) { //1. white (warm and/or cold)
        stream << "00";
        }
        if(NumColors >= 5) { //2. white (warm and/or cold)
        stream << "00";
        }
    }
    int i =0;
    while(stream.str()[i] != '\n' || stream.str()[i] != '\r') {
        mqttStream[i] = stream.str()[i];
    }
}
