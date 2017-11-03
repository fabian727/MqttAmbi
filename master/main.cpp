#ifdef LinuxX11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#include <iostream>
#include <sys/types.h>
#include <vector>

#include "type.h"
#include "draw_grid.h"
#include "MqttClient.h"

typedef int (*handler)(Display *, XErrorEvent *);

//define for measurement of needed time of loop
#undef TIME
//define, if you want to send the data as String over MQTT, else undef. The data will be sent binary
#undef MQTT_STRING

//used for debugging purposes, like drawing an image of the screen or average calculated colours
#undef DEBUG


#ifdef LinuxX11
int handleX11Error(Display *d, XErrorEvent *er)
{
   printf("X11 Error: %i \n", er->error_code);
   std::cout << "X11 Error: " << er->error_code << std::endl;

   return er->error_code;
}
#endif

/*
 *
 */
Color CalcAverageColor(XImage *image, int xOffset, int yOffset, int width, int height) {
	Color color = {0};
	int r,g,b;
	for(int y = yOffset; y < yOffset+height; y++) {
		for(int x = xOffset; x < xOffset+width; x++) {
			r = (XGetPixel(image,x,y) & image->red_mask) >> 16;
			g = (XGetPixel(image,x,y) & image->green_mask) >> 8;
			b = XGetPixel(image,x,y) & image->blue_mask;
				color.red += r;
				color.green += g;
				color.blue += b;
	}	}
	color.red = color.red / (width*height);
	color.green = color.green / (width*height);
	color.blue = color.blue/ (width*height);

	/*maximize colour brightness */
	/*
	if(color.red > color.green && color.red > color.blue) {
		color.green = color.green/color.red*255;
		color.blue = color.blue/color.red*255;
		color.red = 255;
	} else 	if(color.green > color.red && color.green > color.blue) {
		color.red = color.red/color.green*255;
		color.blue = color.blue/color.green*255;
		color.green = 255;
	} else 	if(color.blue > color.green && color.blue > color.red) {
		color.green = color.green/color.blue*255;
		color.red = color.red/color.blue*255;
		color.blue = 255;
	}
	*/
	return color;
}

#define NumColors 4
using namespace std;

/* ToDo: SetUp last Will, all LEDS go black */

int main()
{

/* setup mqtt */
	int NumLeds = 32;
	int rc=0;
	string topic = "/desk/backlight";

    MqttClient * mqtt;
	mqtt = new MqttClient("desktop","openhab.local",1883);

	/* configure number of colors */

	/* configure number of leds */
//	mqtt->confNumLeds();

	/*set last will*/
//	mqtt->will_set(&topic[0],NumLeds*NumColors,&mqttStream[0]);

#ifdef MQTT_STRING
	std::stringstream stream;
	char stringbuffer[NumLeds*NumColors];
   	string mqttStream;
#else
 	uint8_t mqttStream[NumLeds*NumColors];
#endif




#ifdef LinuxX11
 	/* setup x11 */
	Display *display = XOpenDisplay(NULL);
	Screen  *screen  = DefaultScreenOfDisplay(display);
	XImage *image;
	XSetErrorHandler(&handleX11Error);
#elif Windows
#elif LinuxWayland
#else
#endif
	Color temp[NumLeds*2+2];
	Color averageColor[NumLeds];

	while(true) {
#ifdef TIME
   auto start = std::chrono::high_resolution_clock::now();
#endif


#ifdef LinuxX11
   	   image = XGetImage(display,RootWindow (display, DefaultScreen (display)), 0,0 , screen->width,screen->height,AllPlanes, ZPixmap);
#elif Windows
#elif LinuxWayland
#else
#endif
   	   for(int leds=0; leds < NumLeds+2;leds++) {
		   temp[leds] = CalcAverageColor(image,screen->width/NumLeds*leds,20,screen->width/NumLeds,screen->height-20);
   	   }

   	   for(int leds=1;leds < NumLeds+1;leds++) {
   		   averageColor[leds-1].red = (temp[leds-1].red + temp[leds].red + temp[leds+1].red)/3;
			averageColor[leds-1].green = (temp[leds-1].green + temp[leds].green + temp[leds+1].green)/3;
			averageColor[leds-1].blue = (temp[leds-1].blue + temp[leds].blue + temp[leds+1].blue)/3;
   	   }

#ifdef MQTT_STRING
/* string transfer, every 2 bytes/chars equals 1 hex number/color */
   stream.str("");
	for(int leds=0;leds<NumLeds;leds++) {
		stream << std::setfill ('0') << std::setw(sizeof('T')*2)
				<< std::hex << averageColor[leds].green;
		stream << std::setfill ('0') << std::setw(sizeof('T')*2)
				<< std::hex << averageColor[leds].red;
		stream << std::setfill ('0') << std::setw(sizeof('T')*2)
				<< std::hex << averageColor[leds].blue;
		stream << "00";
	}
	mqttStream = stream.str();
	mqtt->publish(NULL,"/desk/backlight" + "/colour",NumLeds*4*2,&mqttStream[0]);
 	if(mqtt->loop()) {
		mqtt->reconnect();
	}
#else
/* binary transfer, every byte/char equals 1 hex number/color */
 	mqtt->createByteStream(NumLeds,NumColors,&mqttStream[0],&averageColor[0]);

 	string subtopic = topic + "/colour";
 	rc = mqtt->publish(NULL,&subtopic[0],(NumLeds*NumColors),&mqttStream[0]);
 	if(rc == MOSQ_ERR_NOMEM) {
 		printf("MQTT publish: out of memory\n");
 	} else if(rc == MOSQ_ERR_PAYLOAD_SIZE) {
 		printf("MQTT publish: too long payload\n");
 	} else if(rc == MOSQ_ERR_INVAL) {
 		printf("MQTT publish: input parameters invalid\n");
 	}
 	if(mqtt->loop()) {
		mqtt->reconnect();
	}
#endif //MQTT_STRING

#ifdef TIME
    auto finish = std::chrono::high_resolution_clock::now();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);
	std::cout << "xGetImage: " << microseconds.count() << " µs\n";
#endif //TIME

#ifdef DEBUG
		draw_led_grid(averageColor,NumLeds,1);
		draw_grid(image);
#endif //DEBUG

#ifdef LinuxX11
		XDestroyImage(image);
#elif Windows
#elif LinuxWayland
#else
#endif
	}

	return 0;
}
