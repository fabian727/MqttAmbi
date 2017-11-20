#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <color_wheel.hpp>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QRgb>
#include <QScreen>
#include <QStatusBar>
#include <QWidget>
#include <QSignalMapper>

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
    QAction *trayActionAmbiOne;
    QAction *trayActionAmbiTwo;
    QAction *trayActionQuit;
    QAction *windowActionQuit;

private:
    ColorWheel  *windowColorWheel;
    QLineEdit *windowLine;

    QAction *windowActionClose;
    QAction *windowActionColor;

    QAction *trayActionOpenOne;
    QAction *trayActionOpenTwo;

    QAction *traySettings;

    QSystemTrayIcon *trayicon;
    QMenu *traymenu;

//protected:

signals:
    void setAmbiOne(bool checked);
    void setAmbiTwo(bool checked);
    void colorChanged(int id, QColor colour);

public slots:

private slots:
    void sOpenOne(void);                  //open window
    void sOpenTwo(void);                 //open window
    void sText(QColor color);           //update textbox in window with current color
    void sColor(void);                 //update colorwheel with color from textbox
};

#endif // MAINWINDOW_H
