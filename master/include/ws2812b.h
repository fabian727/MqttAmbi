#ifndef WS2812B_H
#define WS2812B_H

#include "QColor"
#include <X11/Xlib.h>

class ws2812b
{
public:
    ws2812b(int, int);
    ~ws2812b();
    void CalcAverageColor(QColor *led, XImage *image, int xOffset, int yOffset, int width, int height);
    int getNumLeds(void);
    int getNumColors(void);
private:
    int colors;
    int leds;
};

#endif // WS2812B_H
