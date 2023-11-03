#ifndef SCREEN_H
#define SCREEN_H

#include <linux/fb.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "button.h"
#include "text.h"
#include "mouse.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct{
	int fbFile;
	uint8_t *mmapFramebuffer; // the framebuffer itself
	uint8_t *drawBuffer; // where data is written to before rendere to framebuffer
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	int bpp;
	mouse_t *mouse;
	text_t **text;
	button_t **buttons;
	int textCount;
	int buttonCount;
	int maxTextCount;
	int maxButtonCount;
	bool derived;
} screen_t;

screen_t *createScreen(char *fbFile);
screen_t *deriveScreen(screen_t *screen); // facilitates the creation of windows by having two screens on one framebuffer (only one can be rendered at a time so calm down)
void closeScreen(screen_t *screen);
void addText(text_t* text, screen_t *screen);
void addButton(button_t* button, screen_t *screen);
void handleInput(screen_t* screen);
void renderTextToScreen(text_t *text, screen_t *screen);
void renderButtonToScreen(button_t *button, screen_t *screen);
void renderMouse(screen_t *screen);
void clearScreen(screen_t *screen);
void renderScreen(screen_t *screen);
void flushScreen(screen_t *screen);

screen_t *createScreen(char *fbFile){
	screen_t *newScreen = (screen_t*)calloc(1,sizeof(screen_t));
	if((newScreen->fbFile = open(fbFile, O_RDWR)) == -1){
		perror("Failed to open framebuffer\n");
		free(newScreen);
		return NULL;
	}
	if(ioctl(newScreen->fbFile, FBIOGET_FSCREENINFO, &newScreen->finfo) == -1){
		perror("Failed reading framebuffer fixed information\n");
		free(newScreen);
		return NULL;
	}
	if(ioctl(newScreen->fbFile, FBIOGET_VSCREENINFO, &newScreen->vinfo) == -1){
		perror("Failed reading framebuffer variable information\n");
		free(newScreen);
		return NULL;
	}
	newScreen->mouse = getMouse();
	newScreen->bpp = newScreen->vinfo.bits_per_pixel/8;
	newScreen->drawBuffer = (uint8_t*)calloc(newScreen->vinfo.xres*newScreen->vinfo.yres*newScreen->bpp,1);
	newScreen->mmapFramebuffer = (uint8_t*)mmap(0, newScreen->vinfo.xres*newScreen->vinfo.yres*newScreen->bpp, PROT_READ | PROT_WRITE, MAP_SHARED, newScreen->fbFile, 0);
	close(newScreen->fbFile);
	newScreen->maxTextCount = 2;
	newScreen->text = (text_t**)calloc(newScreen->maxTextCount, sizeof(text_t*));
	newScreen->textCount = 0;
	newScreen->maxButtonCount = 2;
	newScreen->buttons = (button_t**)calloc(newScreen->maxButtonCount, sizeof(button_t*));
	newScreen->textCount = 0;
	return newScreen;
}

screen_t *deriveScreen(screen_t *screen){ // facilitates the creation of windows by having two screens on one framebuffer (only one can be rendered at a time so calm down)
	screen_t *newScreen = (screen_t*)calloc(1,sizeof(screen_t));
	newScreen->derived = true;
	newScreen->mmapFramebuffer = screen->mmapFramebuffer;
	newScreen->maxTextCount = 2;
	newScreen->textCount = 0;
	newScreen->maxButtonCount = 2;
	newScreen->textCount = 0;
	newScreen->text = (text_t**)calloc(newScreen->maxTextCount, sizeof(text_t*));
	newScreen->buttons = (button_t**)calloc(newScreen->maxButtonCount, sizeof(button_t*));
	newScreen->bpp = screen->bpp;
	newScreen->mouse = screen->mouse;
	memcpy(&newScreen->vinfo, &screen->vinfo, sizeof(screen->vinfo));
	memcpy(&newScreen->finfo, &screen->finfo, sizeof(screen->finfo));
	newScreen->drawBuffer = (uint8_t*)calloc(newScreen->vinfo.xres*newScreen->vinfo.yres*newScreen->bpp,1);
	return newScreen;
}

void closeScreen(screen_t *screen){
	free(screen->drawBuffer);
	for(int i = 0; i < screen->maxTextCount; i++){
		if(screen->text[i]){
			deleteTextElement(screen->text[i]);
		}
	}
	free(screen->text);
	for(int i = 0; i < screen->maxButtonCount; i++)
		if(screen->buttons[i])
			deleteButtonElement(screen->buttons[i]);
	if(!screen->derived)
		closeMouse(screen->mouse);
	free(screen->buttons);
	if(!screen->derived)
		munmap(screen->mmapFramebuffer, screen->vinfo.xres*screen->vinfo.yres*screen->bpp);
	free(screen);
	screen = NULL;
}

void addText(text_t* text, screen_t *screen){
	if(screen->textCount >= screen->maxTextCount){
		screen->maxTextCount *= 2;
		screen->text = (text_t**)realloc(screen->text, screen->maxTextCount*sizeof(text_t*));
		for(int i = screen->textCount; i < screen->maxTextCount; i++)
			screen->text[i] = NULL;
	}
	int newTextLocation = -1;
	while(screen->text[++newTextLocation]);
	screen->text[newTextLocation] = text;
	screen->textCount++;
}

void addButton(button_t* button, screen_t *screen){
	if(screen->buttonCount >= screen->maxButtonCount){
		screen->maxButtonCount *= 2;
		screen->buttons = (button_t**)realloc(screen->buttons, screen->maxButtonCount*sizeof(button_t*));
		for(int i = screen->buttonCount; i < screen->maxButtonCount; i++)
			screen->buttons[i] = NULL;
	}
	int newButtonLocation = -1;
	while(screen->buttons[++newButtonLocation]);
	screen->buttons[newButtonLocation] = button;
	screen->buttonCount++;
}

void handleInput(screen_t* screen){
	updateMouse(screen->mouse);
	screen->mouse->y = MIN(MAX(0,screen->mouse->y), screen->vinfo.yres-screen->mouse->size);
	screen->mouse->x = MIN(MAX(0,screen->mouse->x), screen->vinfo.xres-screen->mouse->size);
	for(int i = 0; i < screen->maxButtonCount; i++){
		if(screen->buttons[i] &&
			screen->buttons[i]->x<=screen->mouse->x &&
			screen->buttons[i]->x+screen->buttons[i]->byteWidth/screen->buttons[i]->bpp>=screen->mouse->x &&
			screen->buttons[i]->y<=screen->mouse->y &&
			screen->buttons[i]->y+screen->buttons[i]->size*10>=screen->mouse->y){
			if(!screen->mouse->justClicked)
				hoverButton(screen->buttons[i]);
			else
				clickButton(screen->buttons[i]);
		} else if(screen->buttons[i] && screen->buttons[i]->currBorderColor != regularBorder)
				resetButton(screen->buttons[i]);
	}
}

void renderTextToScreen(text_t *text, screen_t *screen){
	if(!text->visible)
		return;
	if(text->bpp != screen->bpp){
		text->bpp = screen->bpp;
		regenerateTextBuffer(text);
	}
	bool rerender = true;
	for(int i = 0; i < text->byteWidth; i++)
		if(text->textbuffer[i]){
			rerender = false;
			break;
		}
	if(rerender)
		renderText(text);
	for(int y = text->y; y<screen->vinfo.yres && y<text->y+text->fontSize*8; y++){
		int pos = screen->vinfo.xres*screen->bpp*y+MAX(MIN(text->x,screen->vinfo.xres),0)*screen->bpp;
		memcpy(screen->drawBuffer+pos, text->textbuffer+(y-text->y)*text->byteWidth, MAX(MIN(text->byteWidth, screen->vinfo.xres*screen->bpp-text->x*text->bpp), 0));		
	}
}

void renderButtonToScreen(button_t *button, screen_t *screen){
	if(!button->visible)
		return;
	if(button->bpp != screen->bpp){
		button->bpp = screen->bpp;
		regenerateButtonBuffer(button);
	}
	bool rerender = true;
	for(int i = 0; i < button->byteWidth; i++)
		if(button->buttonbuffer[i]){
			rerender = false;
			break;
		}
	if(rerender)
		renderButton(button);
	for(int y = button->y; y<screen->vinfo.yres && y<button->y+button->size*10; y++){
		int pos = screen->vinfo.xres*screen->bpp*y+MAX(MIN(button->x,screen->vinfo.xres),0)*screen->bpp;
		memcpy(screen->drawBuffer+pos, button->buttonbuffer+(y-button->y)*button->byteWidth, MAX(MIN(button->byteWidth, screen->vinfo.xres*screen->bpp-button->x*button->bpp), 0));		
	}
}

void renderMouse(screen_t *screen){
	mouse_t *mouse = screen->mouse;
	char *mouseBuffer = calloc(mouse->size*screen->bpp,1);
	for(int i = 0; i < mouse->size*screen->bpp; i+=screen->bpp){
		memcpy(mouseBuffer+i, mouse->color, screen->bpp);
	}
	int z = MAX(0, MIN(mouse->size, screen->vinfo.yres-mouse->y));
	for(int i = 0; i < z; i++){
		memcpy(screen->drawBuffer+((i+MAX(0,mouse->y))*screen->bpp*screen->vinfo.xres)+mouse->x*screen->bpp, mouseBuffer, (mouse->size-i)*screen->bpp);
	}
	free(mouseBuffer);
}

void clearScreen(screen_t *screen){
	memset(screen->drawBuffer, 0, screen->vinfo.xres*screen->bpp*screen->vinfo.yres);
}

void renderScreen(screen_t *screen){
	clearScreen(screen);
	handleInput(screen);
	for(int i = 0; i < screen->maxTextCount; i++){
		if(screen->text[i]){
			renderTextToScreen(screen->text[i], screen);
		}
	}
	for(int i = 0; i < screen->maxButtonCount; i++){
		if(screen->buttons[i]){
			renderButtonToScreen(screen->buttons[i], screen);
		}
	}
	renderMouse(screen);
}

void flushScreen(screen_t *screen){
	memcpy(screen->mmapFramebuffer, screen->drawBuffer, screen->vinfo.xres*screen->vinfo.yres*screen->bpp);
	msync(screen->mmapFramebuffer, screen->vinfo.xres*screen->vinfo.yres*screen->bpp, MS_SYNC);
}

#endif
