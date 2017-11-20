#include "ws2812b.h"

#ifdef LinuxX11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

ws2812b::ws2812b(int ld, int clr) {
    leds = ld;
    colors = clr;
}
ws2812b::~ws2812b(void) {

}

int ws2812b::getNumColors(void) {
    return colors;
}

int ws2812b::getNumLeds(void) {
    return leds;
}

/*
 *
 */
void ws2812b::CalcAverageColor(QColor *led, XImage *image, int xOffset, int yOffset, int width, int height) {
    QColor color[leds+2] = {};
    uint tmp_red = 0,
         tmp_green = 0,
         tmp_blue = 0;
    int xStepWidth = (width)/(leds+2);
    for(int num=0;num < leds+2; num++) {
        tmp_red = 0;
        tmp_green = 0;
        tmp_blue = 0;
        for(int y = yOffset; y < height; y++) {
            for(int x = xOffset+xStepWidth*num; x < xOffset+xStepWidth*(num+1); x++) {
                    tmp_red   += (XGetPixel(image,x,y) & image->red_mask)   >> 16;
                    tmp_green += (XGetPixel(image,x,y) & image->green_mask) >> 8;
                    tmp_blue  += (XGetPixel(image,x,y) & image->blue_mask);
        }	}
        color[num].setRgb(tmp_red / (xStepWidth*height),
                          tmp_green / (xStepWidth*height),
                          tmp_blue/ (xStepWidth*height));
    }

    /*maximize colour brightness */
    /*
    if(color.red() > color.green() && color.red() > color.blue()) {
        color.green() = color.green()/color.red()*255;
        color.blue() = color.blue()/color.red()*255;
        color.red() = 255;
    } else 	if(color.green() > color.red() && color.green() > color.blue()) {
        color.red() = color.red()/color.green()*255;
        color.blue() = color.blue()/color.green()*255;
        color.green() = 255;
    } else 	if(color.blue() > color.green() && color.blue() > color.red()) {
        color.green() = color.green()/color.blue()*255;
        color.red() = color.red()/color.blue()*255;
        color.blue() = 255;
    }
    */

    for(int num=0;num < leds;num++) {
        led[num].setRgb((color[num].red() + color[num+1].red() + color[num+2].red())/3,
                        (color[num].green() + color[num+1].green() + color[num+2].green())/3,
                        (color[num].blue() + color[num+1].blue() + color[num+2].blue())/3);
    }
}
