

#include "inc/debug.h"
#include "inc/gui.h"
#include "inc/terminal.h"
#include "inc/config.h"

#include "inc/settings.h"
#include "inc/draw_grid.h"
#include "inc/MqttClient.h"

#include <QApplication>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <string>
#include <QString>
#include <QPixmap>
#include <QScreen>

#include <sys/types.h>
#include <vector>

#include <qmetatype.h>

using namespace color_widgets;
using namespace std;

#include <unistd.h>

int main(int argc, char *argv[])
{
    DEBUG_PRINT("");

    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    //TrayActionMenu
    MainWindow window;

    //SettingsWindow
    SettingsWindow swindow;
    MqttClient mqtt;

    app.connect(&window,SIGNAL(colorChanged(QColor)),&mqtt,SLOT(getColour(QColor)),Qt::DirectConnection);
    app.connect(window.traySettings,SIGNAL(triggered()),&swindow,SLOT(open()),Qt::DirectConnection);

    //connect all settings, which will be saved for the next time
    app.connect(&swindow,SIGNAL(lightNumLeds(uint8_t)),&mqtt,SLOT(lightUpNumLeds(uint8_t)),Qt::DirectConnection);

    app.connect(&swindow,SIGNAL(configFileChanged()),&mqtt,SLOT(loadConfigFile()),Qt::DirectConnection);

    //handle ambi-light-modus (on off)
    app.connect(&swindow,SIGNAL(setAmbi(bool)),&mqtt,SLOT(getAmbi(bool)),Qt::DirectConnection);
    app.connect(&swindow,SIGNAL(setAmbi(bool)),&window,SLOT(getAmbi(bool)),Qt::DirectConnection);
    app.connect(window.trayActionAmbi,SIGNAL(triggered(bool)),&swindow,SLOT(saveAmbi(bool)),Qt::DirectConnection);
    app.connect(window.trayActionAmbi,SIGNAL(triggered(bool)),&mqtt,SLOT(getAmbi(bool)),Qt::DirectConnection);

    //handle quit
    app.connect(window.trayActionQuit,SIGNAL(triggered()),&mqtt,SLOT(getStop()),Qt::DirectConnection);                        //quit by tray menu
    app.connect(window.trayActionQuit,SIGNAL(triggered()),&app,SLOT(quit()),Qt::DirectConnection);                            //quit by tray menu

    app.connect(window.windowActionQuit,SIGNAL(triggered()),&mqtt,SLOT(terminate()),Qt::DirectConnection);                    //quit by Ctrl+Q
    app.connect(window.windowActionQuit,SIGNAL(triggered()),&app,SLOT(quit()),Qt::DirectConnection);                          //quit by Ctrl+Q

    swindow.readFile();
    return app.exec();
}
