#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "../include/dui.h"

bool keepRunning = true;

void quitclick(){
	keepRunning = false;
}

text_t *text = NULL;
input_t *input = NULL;

void copyText(){
	setTextText(text, input->input);
}

int main(int argc, char **argv){
	screen_t *screen = createScreen("/dev/fb0");
	button_t *button = createButtonElement(screen->vinfo.xres, 0, "quit", 2, LT_RED, DK_RED, quitclick, screen->bpp);
	button_t *buttontwo = createButtonElement(0, 32, "copy", 2, LT_BLUE, DK_BLUE, copyText, screen->bpp);
	input = createInputElement(0, 0, 30, TEXT_L, LT_GREEN, DK_GREEN, screen->bpp);
	text = createTextElement(0, 52, "", TEXT_M, LT_YELLOW, DK_YELLOW, screen->bpp);
	button->x -= button->byteWidth/button->bpp;
	addButton(button, screen);
	addInput(input, screen);
	addButton(buttontwo, screen);
	addText(text, screen);
	while(keepRunning){
		renderScreen(screen);
		flushScreen(screen);
	}
	closeScreen(screen);
}

