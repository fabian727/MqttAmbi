#include "../inc/settings.h"
#include "../inc/gui.h"
#include "../inc/config.h"
#include "../../build-controler-Desktop-Debug/ui_settingswindow.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow),
    SettingsFile("SmartHome","MqttAmbi")
{
    QString broker=MQTT_BROKER,
            topic=MQTT_BASE_TOPIC,
            scheme="GRBW";

    uint ledsperdisplay=MQTT_LEDS,
        ledsperstripe=MQTT_LEDS,
        stripes=MQTT_STRIPES;

    SettingsFile.setPath(QSettings::NativeFormat,QSettings::UserScope,"MqttAmbi");

    if(SettingsFile.contains("broker")) {
        broker = SettingsFile.value("broker").toString();
    }
    if(SettingsFile.contains("topic")) {
        topic = SettingsFile.value("topic").toString();
    }
    if(SettingsFile.contains("scheme")) {
        scheme = SettingsFile.value("scheme").toString();
    }
    if(SettingsFile.contains("ledsperdisplay")) {
        ledsperdisplay = SettingsFile.value("ledsperdisplay").toUInt();
    }
    if(SettingsFile.contains("stripes")) {
        stripes = SettingsFile.value("stripes").toUInt();
    }
    if(SettingsFile.contains("ledsperstripe")) {
        ledsperstripe = SettingsFile.value("ledsperstripe").toUInt();
    }

    //set values, which were configured, else default
    ui->setupUi(this);
    ui->broker->setText(broker);
    ui->topic->setText(topic);
    ui->ledsperdisplay->setValue(ledsperdisplay);
    ui->stripes->setValue(stripes);
    ui->ledsperstripe->setValue(ledsperstripe);
    int number = ui->scheme->findText(scheme);
    if(number != -1) {
        ui->scheme->setCurrentIndex(number);
    }
    this->setNumLeds(ledsperstripe,true);
    this->setTopic(topic.toStdString());
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_buttonBox_accepted()
{
    SettingsFile.setValue("broker",ui->broker->text());
    SettingsFile.setValue("topic",ui->topic->text());
    SettingsFile.setValue("scheme",ui->scheme->currentText());
    SettingsFile.setValue("ledsperstripe",ui->ledsperstripe->value());
    SettingsFile.setValue("ledsperdisplay",ui->ledsperdisplay->value());
    SettingsFile.setValue("stripes",ui->stripes->value());

    //watch the correct call order. First call for internal save. Second call for save on the strip (send per mqtt)
    this->setNumLeds(ui->ledsperdisplay->value(),true);
    this->setNumLeds(ui->ledsperstripe->value(),false);
    this->setTopic(ui->topic->text().toStdString());
}

void Settings::updateAmbi() {
    bool ambi=false;
    if(SettingsFile.contains("ambi")) {
        ambi = SettingsFile.value("ambi").toBool();
    }
    this->setAmbi(ambi);
}

void Settings::saveAmbi(bool ambi) {
    SettingsFile.setValue("ambi",ambi);
}

void Settings::on_ledsperstripe_valueChanged(int leds)
{
    saveAmbi(false);
    this->setAmbi(false);
    this->setNumLeds((uint8_t) leds,false);
}

void Settings::on_ledsperdisplay_valueChanged(int leds)
{
    saveAmbi(false);
    this->setAmbi(false);
    this->setNumLeds((uint8_t) leds,true);
}

void Settings::on_buttonBox_rejected()
{
    //watch the correct call order. First call for internal save. Second call for save on the strip (send per mqtt)
    this->setNumLeds((uint8_t) SettingsFile.value("ledsperdisplay").toUInt(),true);
    this->setNumLeds((uint8_t) SettingsFile.value("ledsperstripe").toUInt(),false);

}
