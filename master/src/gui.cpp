#include "gui.h"

#include <QWidgetAction>
#include <QSlider>

using namespace std;
using namespace color_widgets;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setup_win(this);
    setup_tray(this);
}

void MainWindow::setup_win(MainWindow *window) {
    /*set up the main window */
    QScreen *ratio = QGuiApplication::primaryScreen();
    window->setGeometry(ratio->geometry().width()-250,0,250,250);
    window->setFixedSize(250,250);
    window->setObjectName("ColorPicker");

    /* Graphical Objects */
    QWidget     *windowCentral    = new QWidget(window);
    QGridLayout *windowLayout     = new QGridLayout(windowCentral);
    windowColorWheel = new ColorWheel(window);
    windowLine = new QLineEdit(window);

    windowLine->setMaximumWidth(100);

    windowLayout->setObjectName("centralLayout");
    windowLayout->addWidget(windowColorWheel);
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
    connect(windowColorWheel,SIGNAL(colorChanged(QColor)),window,SLOT(sText(QColor)),Qt::DirectConnection);      //if color wheel is changed, change text line
    connect(windowLine,SIGNAL(editingFinished()),window,SLOT(sColor()),Qt::DirectConnection);
    connect(windowLine,SIGNAL(returnPressed()),window,SLOT(sColor()),Qt::DirectConnection);
}

void MainWindow::setup_tray(MainWindow *window) {

    QSize *size = new QSize();
    size->setHeight(50);
    size->setWidth(50);
    QIcon icon;
    QString AppPath = QCoreApplication::applicationDirPath();
    AppPath.append("/../master/icon/monochrome.svg");
    icon.addFile(AppPath, *size);

    /*tray menu*/
    traymenu = new QMenu(window);

    trayActionAmbi = new QAction(traymenu);
    trayActionAmbi->setText("Ambi Light");
    trayActionAmbi->setCheckable(true);

    trayActionOpen = new QAction(traymenu);
    trayActionOpen->setText("change Color");


    trayActionSlider = new QWidgetAction (traymenu);
    traySlider = new QSlider (Qt::Horizontal,traymenu);
    traySlider->setRange(0,255);
    traySlider->setValue(125);
    traySlider->setSingleStep(5);
    traySlider->setPageStep(50);
    traySlider->setVisible(true);
    traySlider->setEnabled(true);
    traySlider->show();

    trayActionSlider->setDefaultWidget(traySlider);

    trayActionQuit = new QAction(traymenu);
    trayActionQuit->setText("Quit RGB");

    traySettings = new QAction(traymenu);
    traySettings->setText("Settings");

    traymenu->addSeparator();
    traymenu->addAction(trayActionAmbi);
    traymenu->addAction(trayActionSlider);
    traymenu->addAction(trayActionOpen);
//    traymenu->addSeparator();
    traymenu->addAction(traySettings);
    traymenu->addAction(trayActionQuit);

    /*tray icon*/
    trayicon = new QSystemTrayIcon(window);

    connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason)));

    printf("trayicon connected\n");
    std::cout << std::flush;


    trayicon->setIcon(icon);
    trayicon->setContextMenu(traymenu);
    trayicon->show();

    /* tray connections */

    connect(trayActionOpen,SIGNAL(triggered()),window,SLOT(sOpen()),Qt::DirectConnection);                              //open color window for light
//    connect(traySettings,SIGNAL(triggered()),window,SLOT(sSettings()),Qt::DirectConnection);                              //open color window for settings
}

void MainWindow::trayIconClicked(QSystemTrayIcon::ActivationReason reason) {
    printf("reason: %d\n",reason);
    std::cout << std::flush;
    traySlider->show();
}

/*
 *
 * Slots
 *
 */
void MainWindow::sOpen() {
    QColor color = windowColorWheel->color();
    sText(color);
    trayActionAmbi->setChecked(false);
    this->show();
}


void MainWindow::sText(QColor color) {
    QString colour = "#";
    QString buffer = "";
    if(buffer.setNum(color.red(),16).length() == 1)
    { buffer.append('0'); }
    colour.append(buffer);
    if(buffer.setNum(color.green(),16).length() == 1)
    { buffer.append('0'); }
    colour.append(buffer);
    if(buffer.setNum(color.blue(),16).length() == 1)
    { buffer.append('0'); }
    colour.append(buffer);
    windowLine->setText(colour);
    emit colorChanged(color);
}

void MainWindow::sColor() {
    QString color = windowLine->text();
    QColor colour;
    colour.setRed( color.mid(1,2).toInt(NULL,16));
    colour.setGreen(color.mid(3,2).toInt(NULL,16));
    colour.setBlue(color.mid(5.2).toInt(NULL,16));
    windowColorWheel->setColor(colour);
    emit colorChanged(colour);
}

void MainWindow::getAmbi(bool checked) {
    trayActionAmbi->setChecked(checked);
}
