#include "../inc/terminal.h"

#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <iostream>


#include <stdio.h>
#include <memory.h>

//ToDo: just copied from old project for correct setup under Linux
//ToDo: just opens terminal USB0. Needed root rights for this
//ToDo: port to windows!

Terminal::Terminal() : QThread() {
#if defined(LinuxX11) || defined(LinuxWayland)
    fd = open("/dev/ttyUSB0", O_RDWR | O_NDELAY | O_NOCTTY);
    struct termios port_settings;      // structure to store the port settings in
    tcgetattr(fd,&port_settings);
    port_settings.c_cc[VMIN]  = 1;            // read doesn't block
    port_settings.c_cc[VTIME] = 0;            // 0.5 seconds read timeout
    port_settings.c_cflag &= ~(PARENB | PARODD);   //set no parity
    port_settings.c_cflag &=  ~CSTOPB;  //1 stop bit (clear it, else you use 2)
    port_settings.c_cflag &=  ~CRTSCTS; //no hardware flow control
    port_settings.c_cflag &= ~CSIZE;    //remove data length
    port_settings.c_cflag |= CS8;       //8 data bits
    port_settings.c_cflag |= CREAD;     //enable receiver
    port_settings.c_cflag |= CLOCAL;    //ignore modem status

//    port_settings.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    port_settings.c_lflag = 0;

    port_settings.c_oflag = 0;

    port_settings.c_iflag =  IGNBRK;
    port_settings.c_iflag &= ~(IXON|IXOFF|IXANY);

    cfsetispeed(&port_settings, B38400);
    cfsetospeed(&port_settings, B38400);

    tcsetattr(fd, TCSANOW, &port_settings);
    fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    AmbiOne = false;
    AmbiTwo = false;
    newDataOne = "";
    oldDataOne = "";
    newDataTwo = "";
    oldDataTwo = "";
    pixels = QGuiApplication::primaryScreen();
    QPixmap pixmap = pixels->grabWindow(0);
    width = pixmap.width();
    height = pixmap.height();
}

void Terminal::send(const char *data) {
    char n = 0;
    fd_set rdfs;
    struct timeval timeout;

    for(unsigned int i = 0; i < strlen(data);i++)
    {
        n = write(fd,&data[i],1);
        usleep(400);
    }
    n = 0;
    while(n != 1)
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = 500;
        FD_ZERO( &rdfs );
        FD_SET( fd, &rdfs );
        n = select(fd + 1, NULL, &rdfs, NULL, &timeout);
        if(n > 0)
        { tcflush(fd,TCOFLUSH); }
}  }

void Terminal::receive(char *data) {
    int n = 1;
    fd_set rdfs;
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    while(n>0)
    {
        //look how much there is to read
        FD_ZERO( &rdfs );
        FD_SET( fd, &rdfs );
        n = select(fd + 1, &rdfs, NULL, NULL, &timeout);

        //read
        memset(&data[0],'a',sizeof(&data));
        data[15] = 0;
        int bytes = read(fd,data,strlen(data));
        data[bytes] = '\0';
    }
}

void Terminal::run() {
    QColor colour;
    while(true)
    {
        if(AmbiOne || AmbiTwo)
        {
            colour = ambi();
        }
        if(AmbiOne)
        {
            newDataOne.erase();
            setColour(1,colour);
        }
        if (newDataOne != oldDataOne)
        {
            send(&newDataOne[0]);
            oldDataOne = newDataOne;
        }
        if(AmbiTwo)
        {
            newDataTwo.erase();
            setColour(2,colour);
        }
        if (newDataTwo != oldDataTwo)
        {
            this->send(&newDataTwo[0]);
            oldDataTwo = newDataTwo;
        }
        msleep(15);
    }
}

Terminal::~Terminal() {
    close(fd);
}

void Terminal::setColour(int id, QColor colour) {
    if(id == 1)
    {
        newDataOne = "";
        newDataOne.append(to_string(id)).append(":").append(to_string(colour.blue())).append(",").append(to_string(colour.green())).append(",").append(to_string((int)(colour.red()*0.7))).append("!\n");
    } else if(id == 2) {
        newDataTwo = "";
        newDataTwo.append(to_string(id)).append(":").append(to_string(colour.red())).append(",").append(to_string(colour.green())).append(",").append(to_string((int)(colour.blue()*0.7))).append("!\n");
    }
}

QColor Terminal::ambi(void) {
    QPixmap pixmap = pixels->grabWindow(0) ;
    QImage screen = pixmap.toImage();
    QColor colour;

    QRgb color;
    int red = 0, green = 0, blue = 0;

    int ygradual = height*0.02;
    int xgradual = width*0.02;
    int i = 0;

    for( int y = height*0.3; y < height*0.7; y+=ygradual)
    {
        for( int x = width*0.35; x < width*0.65; x+=xgradual)
        {
            color  = screen.pixel(x,y);
            red   += qRed(color)*1;
            green += qGreen(color)*0.45;
            blue  += qBlue(color)*0.3;
            i++;
    }   }

    red = red/i;
    if(red > 255)
    {red = 255;}
    green = green/i;
    blue = blue/i;
    colour.setRed(red);
    colour.setGreen(green);
    colour.setBlue(blue);
    return colour;
}

void Terminal::setAmbi(bool checked) {
    AmbiOne = checked;
    string data = "1:255,0,0!\n\r";
    send(&data[0]);
}
