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
//#include <X11/Xutil.h>
#endif
#include <sys/types.h>
#include <vector>

#include "settings.h"
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

    //TrayActionMenu
    MainWindow window;

    //SettingsWindow
    Settings swindow;
    MqttClient mqtt(MQTT_NAME,MQTT_BROKER,MQTT_BASE_TOPIC,MQTT_PORT);

    app.connect(&window,SIGNAL(colorChanged(QColor)),&mqtt,SLOT(getColour(QColor)),Qt::DirectConnection);
    app.connect(window.traySettings,SIGNAL(triggered()),&swindow,SLOT(open()),Qt::DirectConnection);

    //connect all settings, which will be saved for the next time
    app.connect(&swindow,SIGNAL(setNumLeds(int)),&mqtt,SLOT(getNumLeds(int)),Qt::DirectConnection);
    app.connect(&swindow,SIGNAL(setTopic(std::string)),&mqtt,SLOT(getTopic(std::string)),Qt::DirectConnection);
    app.connect(&swindow,SIGNAL(setAmbi(bool)),&mqtt,SLOT(getAmbi(bool)),Qt::DirectConnection);                               //start Ambi Background for light
    app.connect(&swindow,SIGNAL(setAmbi(bool)),&window,SLOT(getAmbi(bool)),Qt::DirectConnection);

    app.connect(window.trayActionAmbi,SIGNAL(triggered(bool)),&swindow,SLOT(saveAmbi(bool)),Qt::DirectConnection);
    app.connect(window.trayActionAmbi,SIGNAL(triggered(bool)),&mqtt,SLOT(getAmbi(bool)),Qt::DirectConnection);

    //handle quit
    app.connect(window.trayActionQuit,SIGNAL(triggered()),&mqtt,SLOT(getStop()),Qt::DirectConnection);                        //quit by tray menu
    app.connect(window.trayActionQuit,SIGNAL(triggered()),&app,SLOT(quit()),Qt::DirectConnection);                            //quit by tray menu

    app.connect(window.windowActionQuit,SIGNAL(triggered()),&mqtt,SLOT(terminate()),Qt::DirectConnection);                    //quit by Ctrl+Q
    app.connect(window.windowActionQuit,SIGNAL(triggered()),&app,SLOT(quit()),Qt::DirectConnection);                          //quit by Ctrl+Q

    swindow.updateAmbi();
    return app.exec();
}
