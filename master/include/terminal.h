#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <fcntl.h>
#include <termios.h>
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QColor>
#include <QThread>

using namespace std;

class Terminal : public QThread
{
    Q_OBJECT
public:
    explicit Terminal();
    ~Terminal();
private:
    void send(const char *data);
    void receive(char *data);
    int fd;
    bool AmbiOne;
    string oldDataOne;
    string newDataOne;
    bool AmbiTwo;
    string oldDataTwo;
    string newDataTwo;
    void run();
    QColor ambi(void);

    int width;
    int height;
    QScreen *pixels;

public slots:
    void setColour(int id, QColor colour);
    void setAmbiOne(bool checked);
    void setAmbiTwo(bool checked);
};

#endif // TERMINAL_H
