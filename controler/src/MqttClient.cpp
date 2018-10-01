#include "inc/debug.h"
#include "inc/config.h"
#include "inc/MqttClient.h"
#include "inc/settings.h"

#include <QColor>
#include <string.h>
#include <vector>
#include <sstream>
#include <QSettings>
#include <QHostInfo>

#include <sys/types.h>

#include <iostream>

/*
 * if your compilation won't work
 * add following line to your moc_mqttclient.cpp
 * #undef Bool //X11 problems...
*/
//#include "moc_MqttClient.cpp"

using namespace std;

//typedef int (*handler)(Display *, XErrorEvent *);
int handleX11Error(Display *display, XErrorEvent *error) {
    DEBUG_PRINT("");
    printf("X11 Error: %i \n", error->error_code);
    return error->error_code;
}

//setup mosquitto with the name of the computer its running on
MqttClient::MqttClient() : QThread(), mosquittopp(QHostInfo::localHostName().toStdString().c_str()) {
    DEBUG_PRINT("");
    mosqpp::lib_init();			// Initialize libmosquitto

    int port = MQTT_PORT;

    //load default values
    string scheme = SCHEME;
    this->colors  = scheme.length();
    this->leds    = MQTT_LEDS;
    this->colors  = MQTT_COLORS;
    this->topic   = MQTT_BASE_TOPIC;
    this->broker  = MQTT_BROKER;

    loadConfigFile();

    //int timer for ambi light
    //this timer will call function "ambi", which calculates the color of the background
    timer.setTimerType(Qt::PreciseTimer);       //be ms accurate
    timer.setInterval(10);                      //every 10 ms = 10 Hertz
    timer.setSingleShot(true);
    QObject::connect(&this->timer, SIGNAL(timeout()), this, SLOT(TimerHandlerFunction()));
    timer.start();

    //handle X11 display/image handling
    display = XOpenDisplay(NULL);
    screen  = DefaultScreenOfDisplay(display);
    XSetErrorHandler(&handleX11Error);

    int keepalive = 20; // seconds
    this->mosquittopp::connect(broker.toStdString().c_str(),port,keepalive);		// Connect to MQTT Broker
}

MqttClient::~MqttClient() {
    DEBUG_PRINT("");
    XCloseDisplay(display);
    this->mosquittopp::disconnect();
}

void MqttClient::ambi() {
    DEBUG_PRINT("");
    //first byte for configuration. rest for LED colours, white is kept as backlight
    uint8_t mqttStream[1+(3*this->leds)] = {};
    /* binary transfer, every byte/char equals 1 hex number/color */
    mqttStream[0] = BYTE_RGB;        //just transfer red,green,blue, keep old white as backlightning
    /* setup x11 */
    string clrtopic = this->topic.toStdString() + MQTT_TOPIC_COLOUR;

    image = XGetImage(display,RootWindow (display, DefaultScreen (display)), 0,0 , screen->width,screen->height,AllPlanes, ZPixmap);
    this->CalcAverageColor(&mqttStream[1],image,0,20,screen->width,screen->height-20);
    //just set RGB colour and keep white as is (so only 3 byte/led)
    this->publish(NULL,&clrtopic[0],1+(this->leds*3),&mqttStream[0]);
    XDestroyImage(image);
}

void MqttClient::TimerHandlerFunction() {
    DEBUG_PRINT("");
    if(this->loop()) {
        this->reconnect();
    }
    if(running) {
        ambi();
    }
}

void MqttClient::getStop() {
    DEBUG_PRINT("");

    timer.stop();
    wait(30000);
}


/*
 * x/y Offset is taken from the given image
 * width, height too. So you have to calculate the Offset of the left by yourself and tell this function
 * at width and height, you need your offset of the right/bottom self calculated too
 */
void MqttClient::CalcAverageColor(uint8_t *buffer, XImage *image, int xOffset, int yOffset, int width, int height) {
    DEBUG_PRINT("");




    /*
    int xStepWidth = (width)/(this->leds);            //"window"-width of each pixel column
    int number = (xStepWidth*(height-yOffset)); //number of pixels added
    int offset = 0;                             //startoffset for each row
    uint tmp_red = 0,
         tmp_green = 0,
         tmp_blue = 0;

    //take for each led an own row (top to bottom)
    for(int num=0 ;num < this->leds; num++) {
        tmp_green = 0;
        tmp_blue = 0;
        for(int y = yOffset; y < height; y++) {
            //calculate y-offset + which row to go down (start of)
            offset = y * image->bytes_per_line + ((xOffset+(xStepWidth*num)) << 2);
            //go until you got to end of row, then go to next line
            while(offset < y * image->bytes_per_line + ((xOffset+(xStepWidth*(num+1))) << 2)) {
                //each 32 bit is a color in the scheme of blue, green, red
                tmp_blue  += (uint8_t & ) image->data[offset++];
         QMAKE_CXXFLAGS_RELEASE -= -O2        tmp_green += (uint8_t & ) image->data[offset++];
                tmp_red   += (uint8_t & ) image->data[offset++];
                offset++;
        }	}

        //calculate the average of the sum
        *buffer++ = tmp_green / number;
        *buffer++ = tmp_red   / number;
        *buffer++ = tmp_blue  / number;
    }
    */






    int tmp_red[leds] = {},
        tmp_green[leds] = {},
        tmp_blue[leds] = {};
    uint max = 0;
    max = ~max;
    max = max>>1;
    max -= 100;
    int columnwidth = image->bytes_per_line/leds;
    int colour = yOffset * image->bytes_per_line;     //defaults to 0



    while(colour < (height * image->bytes_per_line)) {
        int led = ((colour%image->bytes_per_line)/columnwidth);
        //each 32 bit is a color in the scheme of blue, green, red
        tmp_blue[led]  += (uint8_t & ) image->data[colour++];
        tmp_green[led] += (uint8_t & ) image->data[colour++];
        tmp_red[led]   += (uint8_t & ) image->data[colour++];
        colour++;
    }

#define ALL ((image->bytes_per_line>>2)*(height-yOffset-750))
//#define ALL ((image->bytes_per_line>>2)*height)

    int led=0;
    colour = 0;
    while(led<leds*3) {
        buffer[led++] = tmp_green[colour]/ALL;
        buffer[led++] = tmp_red[colour]/ALL;
        buffer[led++] = tmp_blue[colour]/ALL;
        colour++;
    }




/*
    //xOffset will be ignored for full. Maybe faster, as no recalculation is needed and cache can be preloaded correctly
    int columnWidth = (width/leds)<<2;
    int i = yOffset*image->bytes_per_line, led=0;
    int bigbuffer[leds*3] = {};
    while(i < (height+yOffset) * image->bytes_per_line) {

//    while(i<image->bytes_per_line<<2*height+(yOffset*image->bytes_per_line<<2)) {
        led = (i%(image->bytes_per_line<<2))/(columnWidth<<2);
        bigbuffer[led++] += (uint8_t & ) image->data[i++];
        bigbuffer[led++] += (uint8_t & ) image->data[i++];
        bigbuffer[led++] += (uint8_t & ) image->data[i++];
        i++;
    }


    led=0;
    while(led<leds*3) {
        buffer[led] = bigbuffer[led++]/1;
        buffer[led] = bigbuffer[led++]/1;
        buffer[led] = bigbuffer[led++]/1;
    }

*/




/*


    for(int line = yOffset; line < height; line++) {
        int y = line*image->bytes_per_line;
        int x = xOffset<<2;
        while(x < width<<2) {
            int activeColumn = x/columnWidth;
            buffer[activeColumn]+= (uint8_t & ) image->data[y+x];
            x++;
            buffer[activeColumn]+= (uint8_t & ) image->data[y+x];
            x++;
            buffer[activeColumn]+= (uint8_t & ) image->data[y+x];
            x+=2;
        }
    }
    */
}

//this slot is called, on change of "Ambilight" on the gui
void MqttClient::getAmbi(bool checked) {
    DEBUG_PRINT("");
    running = checked;
/*
    if(checked) {
        timer.start();
    } else {
        timer.stop();
    }
    */
}

//light up just the number of given leds
//be sure, you stoped ambi-light before!
//slot
void MqttClient::lightUpNumLeds(uint8_t numLeds) {
    DEBUG_PRINT("");
    string numledtopic = this->topic.toStdString() + MQTT_TOPIC_NUMLEDS;

    //first set all posible leds to black
    int maxleds = 254;
    this->publish(NULL,&numledtopic[0],1,&maxleds);
    this->getColour(QColor(0,0,0,0));

    //light up the selected number
    this->publish(NULL,&numledtopic[0],1,&numLeds);
    this->getColour(QColor(100,100,100,20));
}


//load the configuration, which is set in the file and interesting for this thread.
//QSettings updates instant all values. It is threadsave
void MqttClient::loadConfigFile() {
    DEBUG_PRINT("");

    QSettings SettingsFile(CONFIG_FILE_PATH,CONFIG_FILE);
    SettingsFile.setPath(QSettings::NativeFormat,QSettings::UserScope,CONFIG_FILE);

    //check the config file for newer values
    if(SettingsFile.contains(TOPIC)) {
        this->topic = SettingsFile.value(TOPIC).toString();
    }
    if(SettingsFile.contains(SCHEME)) {
        QString scheme = SettingsFile.value(SCHEME).toString();
        this->colors = scheme.length();
    }
    //ledsperstripe will be saved in the stripe itself, so no need in here
    if(SettingsFile.contains(LEDSPERDISPLAY)) {
        this->leds = SettingsFile.value(LEDSPERDISPLAY).toUInt();
    }
    if(SettingsFile.contains(BROKER)) {
        this->broker = SettingsFile.value(BROKER).toString();
    }
}


//public slot

void MqttClient::getColour(QColor avgcolour) {
    DEBUG_PRINT("");
    timer.stop();
    uint8_t mqttStream[5];
    string clrtopic = this->topic.toStdString() + MQTT_TOPIC_COLOUR;
    mqttStream[0] = RGBW_INT;
    mqttStream[1] = avgcolour.red();
    mqttStream[2] = avgcolour.green();
    mqttStream[3] = avgcolour.blue();
    mqttStream[4] = avgcolour.alpha();
    this->publish(NULL,clrtopic.c_str(),5,&mqttStream[0]);
}
