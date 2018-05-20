#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QString>
#include <string>
#include <QSettings>

namespace Ui {
class SettingsWindow;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    void updateAmbi();

private:
    Ui::SettingsWindow *ui;
//    QFile SettingsFile;
    QSettings SettingsFile;

private slots:
    void on_buttonBox_accepted();
    void on_ledsperstripe_valueChanged(int arg1);
    void on_ledsperdisplay_valueChanged(int arg1);

    void on_buttonBox_rejected();

public slots:
    void saveAmbi(bool ambi);

signals:
    void setNumLeds(uint8_t leds, bool save);
    void setTopic(std::string topic);
    void setAmbi(bool ambi);

};

#endif // SETTINGSWINDOW_H
