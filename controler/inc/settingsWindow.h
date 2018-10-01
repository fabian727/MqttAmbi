#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QString>
#include <string>

#include "ui_settingswindow.h"

//names, which are saved in the settings-file
#define BROKER "broker"
#define TOPIC  "topic"
#define SCHEME "scheme"
#define LEDSPERSTRIPE "ledsperstripe"
#define LEDSPERDISPLAY "ledsperdisplay"
#define STRIPES "stripes"
#define AMBI "ambi"

//the config-file is stored in linux under /home/user/.config/CONFIG_FILE_PATH/CONFIG_FILE
#define CONFIG_FILE "MqttAmbi"
#define CONFIG_FILE_PATH "SmartHome"

class SettingsWindow : public QDialog, private Ui::SettingsWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    void readFile();

private slots:
    void on_buttonBox_accepted();
    void on_ledsperstripe_valueChanged(int leds);
    void on_ledsperdisplay_valueChanged(int leds);
    void on_buttonBox_rejected();

public slots:
    void saveAmbi(bool ambi);
    void open();

signals:
    void lightNumLeds(uint8_t numLeds);
    void setAmbi(bool ambi);
    void configFileChanged();
};

#endif // SETTINGSWINDOW_H
