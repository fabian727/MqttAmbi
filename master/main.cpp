#include "gui.h"
#include "terminal.h"

#include <QApplication>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <string>
#include <QString>
#include <QPixmap>
#include <QScreen>

#ifdef LinuxX11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#include <sys/types.h>
#include <vector>

#include "type.h"
#include "draw_grid.h"
#include "MqttClient.h"
#include "ws2812b.h"

using namespace color_widgets;
using namespace std;

//define for measurement of needed time of loop
#undef TIME
//define, if you want to send the data as String over MQTT, else undef. The data will be sent binary
#undef MQTT_STRING

//used for debugging purposes, like drawing an image of the screen or average calculated colours
#undef DEBUG

typedef int (*handler)(Display *, XErrorEvent *);
int handleX11Error(Display *display, XErrorEvent *error) {
    printf("X11 Error: %i \n", error->error_code);
   return error->error_code;
}

#include <unistd.h>

int main(int argc, char *argv[])
{
/*
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    MainWindow window;
    terminal serial;

    app.connect(window.trayActionAmbiOne,SIGNAL(triggered(bool)),&serial,SLOT(setAmbiOne(bool)),Qt::DirectConnection);        //start Ambi Background for light 1
    app.connect(window.trayActionAmbiTwo,SIGNAL(triggered(bool)),&serial,SLOT(setAmbiTwo(bool)),Qt::DirectConnection);        //start Ambi Background for light 2
    app.connect(&window,SIGNAL(colorChanged(int,QColor)),&serial,SLOT(setColour(int,QColor)),Qt::DirectConnection);

    app.connect(window.windowActionQuit,SIGNAL(triggered()),&serial,SLOT(terminate()),Qt::DirectConnection);                    //quit by Ctrl+Q
    app.connect(window.windowActionQuit,SIGNAL(triggered()),&app,SLOT(quit()),Qt::DirectConnection);                            //quit by Ctrl+Q
    app.connect(window.trayActionQuit,SIGNAL(triggered()),&serial,SLOT(terminate()),Qt::DirectConnection);                      //quit by tray menu
    app.connect(window.trayActionQuit,SIGNAL(triggered()),&app,SLOT(quit()),Qt::DirectConnection);                              //quit by tray menu

    serial.start();
*/
/* setup ws2812b */
    ws2812b *stripe = new ws2812b(32,4);

/* setup mqtt */
    MqttClient * mqtt = new MqttClient("desktop","openhab.local","desk/backlight",1883);

#ifdef MQTT_STRING
    string mqttStream;
#else
    uint8_t mqttStream[stripe->leds*stripe->colors];
#endif

 	/* setup x11 */
	Display *display = XOpenDisplay(NULL);
	Screen  *screen  = DefaultScreenOfDisplay(display);
	XImage *image;
	XSetErrorHandler(&handleX11Error);
    Color averageColor[stripe->leds];

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
    mqtt->createByteStream(stripe->leds,stripe->colors,&mqttStream[0],&averageColor[0]);

    string subtopic = mqtt->MainTopic + "/colour";
//    mqtt->publish(NULL,&subtopic[0],(stripe->leds*stripe->colors),&mqttStream[0]);


    mqtt->publish(NULL,&subtopic[0],100,&mqttStream[0]);
    usleep(10000); //10ms

#endif //MQTT_STRING
    if(mqtt->loop()) {
        mqtt->reconnect();
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

    return 0;//app.exec();
}
