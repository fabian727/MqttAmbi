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

    int leds=MQTT_LEDS,
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
    if(SettingsFile.contains("leds")) {
        leds = SettingsFile.value("leds").toInt();
    }
    if(SettingsFile.contains("stripes")) {
        stripes = SettingsFile.value("stripes").toInt();
    }

    //set values, which were configured, else default
    ui->setupUi(this);
    ui->broker->setText(broker);
    ui->topic->setText(topic);
    ui->leds->setValue(leds);
    ui->stripes->setValue(stripes);
    int number = ui->scheme->findText(scheme);
    if(number != -1) {
        ui->scheme->setCurrentIndex(number);
    }
    this->setNumLeds(leds);
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
    SettingsFile.setValue("leds",ui->leds->value());
    SettingsFile.setValue("stripes",ui->stripes->value());
    this->setNumLeds(ui->leds->value());
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

void Settings::on_buttonBox_rejected()
{

}

void Settings::on_leds_valueChanged(int leds)
{
    this->setNumLeds(leds);
}
