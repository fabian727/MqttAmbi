#include <QColor>

#include "MqttClient.h"
#include <string.h>
#include <vector>
#include <sstream>

#include <sys/types.h>

#include "config.h"

#ifdef LinuxX11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#include <iostream>

/*
 * if your compilation won't work
 * add following line to your moc_mqttclient.cpp
 * #undef Bool //X11 problems...
*/
//#include "moc_MqttClient.cpp"


using namespace std;

typedef int (*handler)(Display *, XErrorEvent *);
int handleX11Error(Display *display, XErrorEvent *error) {
    printf("X11 Error: %i \n", error->error_code);
   return error->error_code;
}

/* ToDo: SetUp last Will, all LEDS go black */

MqttClient::MqttClient(const char *id, const char *host, const char *topic, int port) : QThread(), mosquittopp(id) {
    mosqpp::lib_init();			// Initialize libmosquitto

    //int timer for ambi light
    //this timer will call function "ambi", which calculates the color of the background
    timer.setTimerType(Qt::PreciseTimer);       //be ms accurate
    timer.setInterval(50);                      //every 50 ms = 20 Hertz
    timer.setSingleShot(true);
    QObject::connect(&this->timer, SIGNAL(timeout()), this, SLOT(TimerHandlerFunction()));
    timer.stop();

    leds=MQTT_LEDS;
    colors=MQTT_COLORS;

    char *msg_buffer = (char *) malloc(leds*colors+1);
    memset(msg_buffer,0,leds*colors+1);

    this->topic = topic;
    int keepalive = 120; // seconds
    mosquittopp::connect(host, port, keepalive);		// Connect to MQTT Broker

/*set last will*/
    string clrtopic = MQTT_TOPIC_COLOUR;
    this->will_set(&clrtopic[0],leds*colors+1,msg_buffer);
}

void MqttClient::ambi() {
    int num_led = leds;
    timer.start();
    //first byte for configuration. rest for LED colours
    uint8_t mqttStream[colors*num_led+1] = {};
    /* binary transfer, every byte/char equals 1 hex number/color */
    mqttStream[0] = BYTE_RGB;        //just transfer red,green,blue
    /* setup x11 */
    Display *display = XOpenDisplay(NULL);
    Screen  *screen  = DefaultScreenOfDisplay(display);
    XImage *image;
    XSetErrorHandler(&handleX11Error);

    string clrtopic = MQTT_TOPIC_COLOUR;

    image = XGetImage(display,RootWindow (display, DefaultScreen (display)), 0,0 , screen->width,screen->height,AllPlanes, ZPixmap);
    this->CalcAverageColor(&mqttStream[1],image,screen->width,20,screen->width,screen->height-20);
    this->publish(NULL,&clrtopic[0],((num_led*3)+1),&mqttStream[0]);
    if(this->loop()) {
        this->reconnect();
    }
    XCloseDisplay(display);
    XDestroyImage(image);
}

void MqttClient::TimerHandlerFunction() {
    ambi();
}

MqttClient::~MqttClient() {
    this->mosquittopp::disconnect();
}

void MqttClient::getStop() {
    timer.stop();
    wait(30000);
}

void MqttClient::CalcAverageColor(uint8_t *buffer, XImage *image, int xOffset, int yOffset, int width, int height) {
    int xStepWidth = (width)/(leds);
    int number = (xStepWidth*(height-yOffset));
    int offset = 0;
    uint tmp_red = 0,
         tmp_green = 0,
         tmp_blue = 0;

    //take for each led an own row (top to bottom)
    for(int num=0 ;num < leds; num++) {
        tmp_red = 0;
        tmp_green = 0;
        tmp_blue = 0;
        for(int y = yOffset; y < height; y++) {
            //calculate y-offset + which row to go down (start of)
            offset = y * image->bytes_per_line + ((xOffset+xStepWidth*num) << 2);
            //go until you got to end of row, then go to next line
            while(offset < y * image->bytes_per_line + ((xOffset+xStepWidth*(num+1)) << 2)) {
                //each 32 bit is a color in the scheme of blue, green, red
                tmp_blue  += (uint8_t & ) image->data[offset++];
                tmp_green += (uint8_t & ) image->data[offset++];
                tmp_red   += (uint8_t & ) image->data[offset++];
                offset++;
        }	}

        //calculate the average of the sum
        *buffer++ = tmp_green / number;
        *buffer++ = tmp_red   / number;
        *buffer++ = tmp_blue  / number;
    }
}

//this slot is called, if "Ambilight" is checked on the gui
void MqttClient::getAmbi(bool checked) {
    running = checked;
    if(checked) {
        timer.start();
    } else {
        timer.stop();
    }
}

//if the number of LEDs is changed, show them directly
void MqttClient::getNumLeds(int leds) {
    bool hasrun = running;
    running = false;
    string subtopic = MQTT_TOPIC_NUMLEDS;
    int maxleds = 254;
    this->publish(NULL,&subtopic[0],1,&maxleds);

    this->getColour(QColor (0,0,0));
    this->leds = leds;
    this->publish(NULL,&subtopic[0],1,&leds);
    this->getColour(QColor (100,100,100));
    running = hasrun;
    if(running) {
        timer.start();
    }
}

void MqttClient::getTopic(std::string maintopic) {
    this->topic = maintopic;
}

void MqttClient::getColour(QColor avgcolour) {
    timer.stop();
    uint8_t mqttStream[5];
    string clrtopic = MQTT_TOPIC_COLOUR;
    mqttStream[0] = RGBW_INT;
    mqttStream[1] = avgcolour.red();
    mqttStream[2] = avgcolour.green();
    mqttStream[3] = avgcolour.blue();
    mqttStream[4] = (mqttStream[1]+mqttStream[2]+mqttStream[3])/3;
    this->publish(NULL,&clrtopic[0],5,&mqttStream[0]);
}
