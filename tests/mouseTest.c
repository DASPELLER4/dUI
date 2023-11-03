#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/dui.h"

void reverse(char s[]){
	int i, j;
	char c;
	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
 }

void itoa(int n, char s[]){
	int i, sign;
	if ((sign = n) < 0)  /* record sign */
		n = -n;          /* make n positive */
	i = 0;
	do {       /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

int main(int argc, char **argv){
	screen_t *screen = createScreen("/dev/fb0");
	text_t *clickedText = createTextElement(0,80,"CLICKED",5,LT_BLUE,DK_BLUE,screen->bpp);
	text_t *xposText = createTextElement(0,120,"",3,LT_GREEN,DK_GREEN,screen->bpp);
	text_t *yposText = createTextElement(0,120,"",3,LT_YELLOW,DK_YELLOW,screen->bpp);
	addText(createTextElement(0,0,"D4MOUSETEST", 10, LT_RED,DK_RED,screen->bpp), screen);
	addText(clickedText, screen);
	addText(xposText, screen);
	addText(yposText, screen);
	xposText->visible = clickedText->visible = yposText->visible = false;;
	char newtext[100] = {0};
	while(1){
		itoa(screen->mouse->x, newtext);
		setTextText(xposText, newtext);
		yposText->x = (xposText->byteWidth/xposText->bpp)+yposText->kerning;
		itoa(screen->mouse->y, newtext);
		setTextText(yposText, newtext);
		if(screen->mouse->clicked)
			clickedText->visible = xposText->visible = yposText->visible = true;
		else
			clickedText->visible = xposText->visible = yposText->visible = false;
		renderScreen(screen);
		flushScreen(screen);
	}
	closeScreen(screen);
}

