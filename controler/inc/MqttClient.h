#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#undef Bool
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted

#include <QThread> //QT needs to be first
#include <QTimer>
#include <QColor>

#include <mosquittopp.h>
#include <iomanip> //uint8_t
#include <string> //string



using namespace std;

#define MQTT_TOPIC_COLOUR  "/colour"
#define MQTT_TOPIC_NUMLEDS "/numleds"

class MqttClient : public QThread, public mosqpp::mosquittopp
{
    Q_OBJECT
public:
    explicit MqttClient();
	~MqttClient();

    QString topic;
    void createByteStream(int NumLeds, int NumColors, uint8_t *mqttStream, QColor *averageColor);
    void createStringStream(int NumLeds,int NumColors,char *mqttStream, QColor *averageColor);
    void maxBrightness();
private:
    bool running;
    void CalcAverageColor(uint8_t *buffer, XImage *image, int xOffset, int yOffset, int width, int height);
    int colors;
    void ambi();
    uint8_t leds;
    QTimer timer;
    QString broker;

    //handle X11 display/image handling
    Display *display;
    Screen  *screen;
    XImage *image;


public slots:
    void getColour(QColor avgcolour);
    void getAmbi(bool checked);
    void lightUpNumLeds(uint8_t leds);
    void getStop();
    void loadConfigFile();
private slots:
    void TimerHandlerFunction();
};

/*
 * decoding scheme for the client
 * SingleLed: each Byte will be sent directly to the LED stripe
 * RGBColor: only a RGB colored stripe is attached
 * RGBWColor: a RGBW colored stripe is attached
 */

#define BYTE_RGB  '9'       //set only RGB as new, keep the old white (as background light)
#define BYTE_WISE '0'       //take array as is and copy paste it
#define RGB_CHAR  '1'       //set all LEDs to rgb and white as 0; sent as Ascii
#define RGBW_CHAR '2'       //set all LEDs to rgbw; sent as Ascii
#define RGB_INT   '3'       //set all LEDs to rgb and white as 0; sent as hex
#define RGBW_INT  '4'       //set all LEDs to this rgbw; sent as hex

#endif // _MQTTCLIENT_H_
