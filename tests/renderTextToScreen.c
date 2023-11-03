#include <stdio.h>
#include <stdlib.h>

#include "../include/dui.h"

int main(int argc, char **argv){
	screen_t *screen = createScreen("/dev/fb0");
	text_t *textone = createTextElement(0, 0, "hello, this is daspeller4s test for text rendering", TEXT_M, LT_BLUE, DK_BLUE, screen->bpp);
	text_t *texttwo = createTextElement(0, 16, "the text above is medium", TEXT_S, LT_BLUE, DK_BLUE, screen->bpp);
	text_t *textthree = createTextElement(0, 24, "here is some large text", TEXT_L, LT_YELLOW, DK_YELLOW, screen->bpp);
	addText(textone, screen);
	addText(texttwo, screen);
	addText(textthree, screen);
	renderTextToScreen(textone, screen);
	renderTextToScreen(texttwo, screen);
	renderTextToScreen(textthree, screen);
	flushScreen(screen);
	closeScreen(screen);
}

