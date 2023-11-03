#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "text.h"

#define hoverBorder 0xA0
#define regularBorder 0x60
#define clickBorder 0xFF

typedef struct buttonStruct{
	char *buttonbuffer;
	char currBorderColor;
	text_t *text;
	void (*onClick)();
	bool visible;
	int bpp;
	int x;
	int y;
	int byteWidth;
	int size;
} button_t;

button_t *createButtonElement(int x, int y, char *text, int size, uint8_t fg[3], uint8_t bg[3], void (*onClick)(), int bpp);
void deleteButtonElement(button_t *button);
void renderButton(button_t* button);
void regenerateButtonBuffer(button_t *button);
void setButtonText(button_t *button, char *text);
void hoverButton(button_t* button);
void clickButton(button_t* button);
void resetButton(button_t* button);

button_t *createButtonElement(int x, int y, char *text, int size, uint8_t fg[3], uint8_t bg[3], void (*onClick)(), int bpp){
	button_t *newButton = (button_t*)calloc(sizeof(button_t),1);
	newButton->bpp = bpp;
	newButton->x = x;
	newButton->y = y;
	newButton->visible = true;
	newButton->text = createTextElement(0, 0, text, size, fg, bg, bpp);
	newButton->onClick = onClick;
	newButton->currBorderColor = regularBorder;
	newButton->size = size;
	newButton->byteWidth = 2*bpp*size+newButton->text->byteWidth;
	newButton->buttonbuffer = calloc(newButton->byteWidth*(2*size+size*8),1);
	return newButton;
}

void deleteButtonElement(button_t *button){
	deleteTextElement(button->text);
	free(button->buttonbuffer);
	free(button);
	button = NULL;
}

void renderButton(button_t* button){
	if(button->text->bpp != button->bpp){
		button->text->bpp = button->bpp;
		regenerateTextBuffer(button->text);
	}
	bool rerender = true;
	for(int i = 0; i < button->text->byteWidth; i++)
		if(button->text->textbuffer[i]){
			rerender = false;
			break;
		}
	if(rerender)
		renderText(button->text);
	memset(button->buttonbuffer, button->currBorderColor, button->byteWidth*(2*button->size+button->size*8));
	for(int i = button->size; i < button->size*9; i++){
		memcpy(button->buttonbuffer+button->size*button->bpp+i*button->byteWidth, button->text->textbuffer+(i-button->size)*button->text->byteWidth, button->text->byteWidth);
	}
}

void regenerateButtonBuffer(button_t *button){
	button->byteWidth = 2*button->bpp*button->size+button->text->byteWidth;
	free(button->buttonbuffer);
	button->buttonbuffer = calloc(button->byteWidth*(10*button->size),1);
}

void setButtonText(button_t *button, char *text){
	setTextText(button->text, text);
	regenerateButtonBuffer(button);
}

void hoverButton(button_t* button){
	button->currBorderColor = hoverBorder;
	renderButton(button);
}

void clickButton(button_t* button){
	button->currBorderColor = clickBorder;
	button->onClick();
	renderButton(button);
}

void resetButton(button_t* button){
	button->currBorderColor = regularBorder;
	renderButton(button);
}

#endif
