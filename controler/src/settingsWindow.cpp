#include "inc/debug.h"
#include "inc/settings.h"
#include "inc/gui.h"
#include "inc/config.h"

#include <QSettings>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent)
{
    DEBUG_PRINT("");
    setupUi(this);
    readFile();
}

//show the window, but reload-config-file and disable ambi-light
void SettingsWindow::open() {
    DEBUG_PRINT("");
    readFile();
    emit setAmbi(false);
    QDialog::open();
}

void SettingsWindow::readFile() {
    DEBUG_PRINT("");

    QSettings SettingsFile(CONFIG_FILE_PATH,CONFIG_FILE);
    SettingsFile.setPath(QSettings::NativeFormat,QSettings::UserScope,CONFIG_FILE);

    //setup default
    QString settingsScheme = MQTT_SCHEME;

    //load config and show it, else show the default value
    if(SettingsFile.contains(BROKER)) {
        broker->setText(SettingsFile.value(BROKER).toString());
    } else {
        broker->setText(MQTT_BROKER);
    }
    if(SettingsFile.contains(TOPIC)) {
        topic->setText(SettingsFile.value(TOPIC).toString());
    } else {
        topic->setText(MQTT_BASE_TOPIC);
    }
    if(SettingsFile.contains(LEDSPERDISPLAY)) {
        ledsperdisplay->setValue(SettingsFile.value(LEDSPERDISPLAY).toUInt());
    } else {
        ledsperdisplay->setValue(NUM_LEDS_PER_DISPLAY);
    }
    if(SettingsFile.contains(STRIPES)) {
        stripes->setValue(SettingsFile.value(STRIPES).toUInt());
    } else {
        stripes->setValue(MQTT_STRIPES);
    }
    if(SettingsFile.contains(LEDSPERSTRIPE)) {
        ledsperstripe->setValue(SettingsFile.value(LEDSPERSTRIPE).toUInt());
    } else {
        ledsperstripe->setValue(NUM_LEDS_PER_STRIPE);
    }

    if(SettingsFile.contains(SCHEME)) {
        settingsScheme = SettingsFile.value(SCHEME).toString();
    }
    int number = scheme->findText(settingsScheme);
    if(number != -1) {
        scheme->setCurrentIndex(number);
    }
}

//private slots (taken if changes are made on the settings in the gui-window
void SettingsWindow::on_ledsperstripe_valueChanged(int leds) {
    DEBUG_PRINT("");
    emit lightNumLeds((uint8_t) leds);
}

void SettingsWindow::on_ledsperdisplay_valueChanged(int leds) {
    DEBUG_PRINT("");
    emit lightNumLeds((uint8_t) leds);
}


//save the new values and light up all leds
//reload the config in all other needed threads
void SettingsWindow::on_buttonBox_accepted() {
    DEBUG_PRINT("");

    QSettings SettingsFile(CONFIG_FILE_PATH,CONFIG_FILE);
    SettingsFile.setPath(QSettings::NativeFormat,QSettings::UserScope,CONFIG_FILE);

    SettingsFile.setValue(BROKER,broker->text());
    SettingsFile.setValue(TOPIC,topic->text());
    SettingsFile.setValue(SCHEME,scheme->currentText());
    SettingsFile.setValue(LEDSPERSTRIPE,ledsperstripe->value());
    SettingsFile.setValue(LEDSPERDISPLAY,ledsperdisplay->value());
    SettingsFile.setValue(STRIPES,stripes->value());

    emit lightNumLeds(ledsperstripe->value());
    emit configFileChanged();
}

//show the old number of configured leds (per stripe) and forget all settings made in the window
void SettingsWindow::on_buttonBox_rejected() {
    DEBUG_PRINT("");
    uint8_t leds = 0;

    QSettings SettingsFile(CONFIG_FILE_PATH,CONFIG_FILE);
    SettingsFile.setPath(QSettings::NativeFormat,QSettings::UserScope,CONFIG_FILE);

    if(SettingsFile.contains(LEDSPERSTRIPE)) {
        leds = SettingsFile.value(LEDSPERSTRIPE).toUInt();
    }
    emit lightNumLeds(leds);
}

//****//
//slot
//****//

//save if the application was stopped during ambi-light or not
void SettingsWindow::saveAmbi(bool ambi) {
    DEBUG_PRINT("");

    QSettings SettingsFile(CONFIG_FILE_PATH,CONFIG_FILE);
    SettingsFile.setPath(QSettings::NativeFormat,QSettings::UserScope,CONFIG_FILE);

    SettingsFile.setValue(AMBI,ambi);
}
