
#include "inc/gui.h"
#include "inc/debug.h"

#include <QWidgetAction>
#include <QSlider>

using namespace std;
using namespace color_widgets;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    DEBUG_PRINT("");
    setup_win(this);
    setup_tray(this);
}

void MainWindow::setup_win(MainWindow *window) {
    DEBUG_PRINT("");
    /*reset the color to black*/
    activeColor.setAlpha(0);
    activeColor.setRed(0);
    activeColor.setGreen(0);
    activeColor.setBlue(0);

    /*set up the main window */
    QScreen *ratio = QGuiApplication::primaryScreen();
    window->setGeometry(ratio->geometry().width()-250,0,250,250);
    window->setFixedSize(250,250);
    window->setObjectName("ColorPicker");

    /* Graphical Objects */
    QWidget     *windowCentral    = new QWidget(window);
    QGridLayout *windowLayout     = new QGridLayout(windowCentral);
    windowColorWheel = new ColorWheel(window);

    windowWhiteSlider = new QSlider(window);
    windowWhiteSlider->setRange(0,255);
    windowWhiteSlider->setOrientation(Qt::Horizontal);
    windowWhiteSlider->setTickInterval(1);
    windowWhiteSlider->setSingleStep(1);
    windowWhiteSlider->setPageStep(10);

    windowLine = new QLineEdit(window);
    windowLine->setMaximumWidth(100);

    windowLayout->setObjectName("centralLayout");
    windowLayout->addWidget(windowColorWheel);
    windowLayout->addWidget(windowWhiteSlider);
    windowLayout->addWidget(windowLine);

    windowCentral->setLayout(windowLayout);

    window->setCentralWidget(windowCentral);

    /* Actions */
    windowActionQuit = new QAction(window);
    windowActionQuit->setShortcut(QKeySequence("Ctrl+Q"));

    windowActionClose = new QAction(window);
    windowActionClose->setShortcut(QKeySequence("Ctrl+W"));

    window->addAction(windowActionQuit);
    window->addAction(windowActionClose);

    /* window connections */
    connect(windowActionClose,SIGNAL(triggered()),window,SLOT(close()),Qt::DirectConnection);                    //close by Ctrl+W

    //colorwheel or slider changed, update the textline
    connect(windowColorWheel,SIGNAL(colorChanged(QColor)),window,SLOT(setLine(void)),Qt::DirectConnection);      //if color wheel is changed, change text line
    connect(windowWhiteSlider,SIGNAL(valueChanged(int)),window,SLOT(setLine(void)),Qt::DirectConnection);

    //the textline was changed, update colorwheel and slider
    connect(windowLine,SIGNAL(editingFinished()),window,SLOT(setColor()),Qt::DirectConnection);
    connect(windowLine,SIGNAL(returnPressed()),window,SLOT(setColor()),Qt::DirectConnection);

}

void MainWindow::setup_tray(MainWindow *window) {
    DEBUG_PRINT("");

    QSize *size = new QSize();
    size->setHeight(50);
    size->setWidth(50);
    QIcon icon;
    QString AppPath = QCoreApplication::applicationDirPath();
    AppPath.append("/../controler/ico/monochrome.svg");
    icon.addFile(AppPath, *size);

    /*tray menu*/
    traymenu = new QMenu(window);

    trayActionAmbi = new QAction(traymenu);
    trayActionAmbi->setText("Ambi Light");
    trayActionAmbi->setCheckable(true);

    trayActionOpen = new QAction(traymenu);
    trayActionOpen->setText("change Color");

    traySettings = new QAction(traymenu);
    traySettings->setText("Settings");

    trayActionQuit = new QAction(traymenu);
    trayActionQuit->setText("Quit RGB");

    traymenu->addSeparator();
    traymenu->addAction(trayActionAmbi);
    traymenu->addAction(trayActionOpen);
    traymenu->addAction(traySettings);
    traymenu->addAction(trayActionQuit);

    /*tray icon*/
    trayicon = new QSystemTrayIcon(window);

    trayicon->setIcon(icon);
    trayicon->setContextMenu(traymenu);
    trayicon->show();

    /* tray connections */

    connect(trayActionOpen,SIGNAL(triggered()),window,SLOT(sOpen()),Qt::DirectConnection);                              //open color window for light
//    connect(traySettings,SIGNAL(triggered()),window,SLOT(sSettings()),Qt::DirectConnection);                              //open color window for settings
}

/*
 *
 * Slots
 *
 */

//if window is new opened
void MainWindow::sOpen(void) {
    DEBUG_PRINT("");
    QColor color = windowColorWheel->color();
    color.setAlpha(windowWhiteSlider->value());
    setLine();
    trayActionAmbi->setChecked(false);
    windowLine->setFocus(Qt::NoFocusReason);
    this->show();
}


//colorwheel or slider changed, reload the textline
void MainWindow::setLine(void) {
    DEBUG_PRINT("");
    activeColor = windowColorWheel->color();
    int white = windowWhiteSlider->value();
    if(white > 255) {
        white = 255;
    } else if(white < 0) {
        white = 0;
    }
    activeColor.setAlpha(white);

    //setup the string. If one color has only 1 digit, prepend a zero.
    QString linecolor = "#";
    QString buffer = "";
    if(buffer.setNum(activeColor.red(),16).length() == 1) {
        linecolor.append('0');
    }
    linecolor.append(buffer);
    if(buffer.setNum(activeColor.green(),16).length() == 1) {
        linecolor.append('0');
    }
    linecolor.append(buffer);
    if(buffer.setNum(activeColor.blue(),16).length() == 1) {
        linecolor.append('0');
    }
    linecolor.append(buffer);
    if(buffer.setNum(activeColor.alpha(),16).length() == 1) {
        linecolor.append('0');
    }
    linecolor.append(buffer);
    windowLine->setText(linecolor);
    emit colorChanged(activeColor);
}

//textline changed, update colorwheel and slider
void MainWindow::setColor(void) {
    DEBUG_PRINT("");
    QString color = windowLine->text();
    activeColor.setRed(  color.mid(1,2).toInt(NULL,16));
    activeColor.setGreen(color.mid(3,2).toInt(NULL,16));
    activeColor.setBlue( color.mid(5,2).toInt(NULL,16));
    activeColor.setAlpha(color.mid(7,2).toInt(NULL,16));
    windowWhiteSlider->setValue(activeColor.alpha());
    windowColorWheel->setColor(activeColor);
    emit colorChanged(activeColor);
}

void MainWindow::getAmbi(bool checked) {
    DEBUG_PRINT("");
    trayActionAmbi->setChecked(checked);
}
