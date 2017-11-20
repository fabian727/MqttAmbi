
#ifndef _draw_grid_h_
#define _draw_grid_h_

#include <QColor>
#include <X11/Xlib.h>

    void draw_grid(XImage* image);
    void draw_led_grid(QColor *array, int width, int height);

#endif
