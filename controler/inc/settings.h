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
    void on_buttonBox_rejected();
    void on_leds_valueChanged(int leds);

public slots:
    void saveAmbi(bool ambi);

signals:
    void setNumLeds(int leds);
    void setTopic(std::string topic);
    void setAmbi(bool ambi);

};

#endif // SETTINGSWINDOW_H
