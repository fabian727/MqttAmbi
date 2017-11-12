#ifndef WS2812B_H
#define WS2812B_H

#include "type.h"


class ws2812b
{
public:
    ws2812b(int, int);
    ~ws2812b();
    int colors;
    int leds;
    void CalcAverageColor(Color *led, XImage *image, int xOffset, int yOffset, int width, int height);
};

#endif // WS2812B_H
