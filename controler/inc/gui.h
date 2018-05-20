#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include "dep/Qt-Color-Widgets/include/QtColorWidgets/color_wheel.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QRgb>
#include <QScreen>
#include <QStatusBar>
#include <QWidget>
#include <QSignalMapper>

#include <QWidgetAction>
#include <QSlider>

#include <string>
#include <iostream>
#include <fstream>

using namespace color_widgets;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void setup_win (MainWindow *window);
    void setup_tray (MainWindow *window);
    QAction *trayActionAmbi;
    QAction *trayActionQuit;
    QAction *windowActionQuit;
    QAction *traySettings;

private:
    ColorWheel  *windowColorWheel;
    QSlider *windowWhiteSlider;
    QLineEdit *windowLine;

    QAction *windowActionClose;
    QAction *windowActionColor;
    QAction *trayActionOpen;

    QMenu *traymenu;
    QSystemTrayIcon *trayicon;

    QColor activeColor;
//protected:

signals:
    void setAmbi(int checked);
    void colorChanged(QColor colour);

public slots:
    void getAmbi(bool checked);

private slots:
    void sOpen(void);                  //open window
    void sText(QColor color);           //update textbox in window with current color
    void sColor(void);                 //update colorwheel with color from textbox
    void sColor(int);                 //update colorwheel with color from textbox
};

#endif // MAINWINDOW_H
