#include "gui.h"
#include "terminal.h"

#include "config.h"

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

#include "draw_grid.h"
#include "MqttClient.h"
#include "ws2812b.h"

using namespace color_widgets;
using namespace std;


#include <unistd.h>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    MainWindow window;
    Terminal serial;
    MqttClient mqtt(MQTT_NAME,MQTT_BROKER,MQTT_BASE_TOPIC,MQTT_PORT);

//    app.connect(window.trayActionAmbiOne,SIGNAL(triggered(bool)),&serial,SLOT(setAmbiOne(bool)),Qt::DirectConnection);        //start Ambi Background for light 1
//    app.connect(window.trayActionAmbiTwo,SIGNAL(triggered(bool)),&serial,SLOT(setAmbiTwo(bool)),Qt::DirectConnection);        //start Ambi Background for light 2
//    app.connect(&window,SIGNAL(colorChanged(int,QColor)),&serial,SLOT(setColour(int,QColor)),Qt::DirectConnection);

//    app.connect(window.windowActionQuit,SIGNAL(triggered()),&serial,SLOT(terminate()),Qt::DirectConnection);                    //quit by Ctrl+Q
    app.connect(window.windowActionQuit,SIGNAL(triggered()),&app,SLOT(quit()),Qt::DirectConnection);                            //quit by Ctrl+Q
//    app.connect(window.trayActionQuit,SIGNAL(triggered()),&serial,SLOT(terminate()),Qt::DirectConnection);                      //quit by tray menu
    app.connect(window.trayActionQuit,SIGNAL(triggered()),&app,SLOT(quit()),Qt::DirectConnection);                              //quit by tray menu

    serial.start();
    mqtt.start();

    return app.exec();
}
