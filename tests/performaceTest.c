#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "../include/dui.h"

int main(int argc, char **argv){
	screen_t *screen = createScreen("/dev/fb0");
	for(int j = 0; j < 1; j++)
		for(int i = 0; i < 1920; i++)
			addText(createTextElement(i,j,"AAA",TEXT_S,(uint8_t[]){rand()%256,rand()%256,rand()%256}, (uint8_t[]){rand()%256,rand()%256,rand()%256}, screen->bpp), screen);
	clock_t begin = clock();
	double frames = 0;
	while(frames < 1000){
		frames++;
		renderScreen(screen);
		flushScreen(screen);
	}
	clock_t end = clock();
	printf("%f FPS\n", frames/((double)(end-begin)/CLOCKS_PER_SEC));
	closeScreen(screen);
}

