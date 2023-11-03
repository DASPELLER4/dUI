#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

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

bool keepRunning = true;
int counter = 0;

void quitclick(){
	keepRunning = false;
}

void counterclick(){
	counter++;
}

int main(int argc, char **argv){
	screen_t *screen = createScreen("/dev/fb0");
	button_t *button = createButtonElement(screen->vinfo.xres, 0, "quit", 2, LT_RED, DK_RED, quitclick, screen->bpp);
	button_t *buttontwo = createButtonElement(0, 0, "Counter", 3, LT_YELLOW, DK_YELLOW, counterclick, screen->bpp);
	button->x -= button->byteWidth/button->bpp;
	text_t *text = createTextElement(0, 6+8*3, "", 5, LT_GREEN, DK_GREEN, screen->bpp);
	addButton(button, screen);
	addButton(buttontwo, screen);
	addText(text, screen);
	char newtext[100] = {0};
	clock_t begin = clock();
	double frames = 0;
	while(keepRunning){
		frames++;
		itoa(counter, newtext);
		setTextText(text, newtext);
		renderScreen(screen);
		flushScreen(screen);
	}
	clock_t end = clock();
	printf("%.6f FPS\n", frames/((double)(end-begin)/CLOCKS_PER_SEC));
	closeScreen(screen);
}

