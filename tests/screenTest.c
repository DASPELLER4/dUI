#include <stdio.h>
#include <stdlib.h>

#include "../include/dui.h"

int main(int argc, char **argv){
	screen_t *screen = createScreen("/dev/fb0");
	for(int y = 0; y < 255; y++)
		for(int i = 0; i < 255; i++){
			screen->drawBuffer[i*4+y*screen->vinfo.xres*screen->bpp] = i;
			screen->drawBuffer[i*4+y*screen->vinfo.xres*screen->bpp+1] = y;
			screen->drawBuffer[i*4+y*screen->vinfo.xres*screen->bpp+2] = 255-y;
		}
	flushScreen(screen);
	closeScreen(screen);
}

