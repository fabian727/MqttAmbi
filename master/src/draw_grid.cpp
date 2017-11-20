#include "draw_grid.h"

#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

void draw_grid (XImage* image) {
	FILE * file = NULL;
	char filename[64];
	sprintf(filename,"picture.ppm");
	file = fopen(filename,"w");
	if(file == NULL) {
		printf("couldn't create picture file\n");
		return;
	}

	fprintf(file,"P3\n");
	fprintf(file,"%i %i\n",image->width,image->height);
	fprintf(file,"255 #max value\n");

	int x,y;
	for(y=0;y<image->height;y++) {
		for(x=0;x<image->width;x++) {
			fprintf(file,"%lu ",(XGetPixel(image,x,y) & image->red_mask ) >> 16);
			fprintf(file,"%lu ",(XGetPixel(image,x,y) & image->green_mask ) >> 8);
		    fprintf(file,"%lu",XGetPixel(image,x,y) & image->blue_mask);
		    fprintf(file,"\n");
		}
	}

	fclose(file);
}

void draw_led_grid (QColor *array, int width, int height) {
	FILE * file = NULL;
	char filename[64];
	sprintf(filename,"leds.ppm");
	file = fopen(filename,"w");
	if(file == NULL) {
		printf("couldn't create picture file\n");
		return;
	}

	fprintf(file,"P3\n");
	fprintf(file,"%i %i\n",width,height);
	fprintf(file,"255 #max value\n");

	int x,y;
	for(y=0;y<height;y++) {
		for(x=0;x<width;x++) {
            fprintf(file,"%i ",array[y*width+x].red());
            fprintf(file,"%i ",array[y*width+x].green());
            fprintf(file,"%i ",array[y*width+x].blue());
		    fprintf(file,"\n");
		}
	}

	fclose(file);
}
