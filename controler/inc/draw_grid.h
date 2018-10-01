#ifndef _draw_grid_h_
#define _draw_grid_h_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#undef Bool
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted

#include <QColor>

void draw_grid(XImage* image);
void draw_led_grid(QColor *array, int width, int height);

#endif
