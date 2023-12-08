#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <stdlib.h>

#include "text.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct{
	int cursor;
	char *input;
	char *tempText;
	int maxInputLength;
	text_t *text;
	int width;
	void (*onReturn)();
	void (*onKeyPress)();
	bool focused;
	bool visible;
	int x;
	int y;
} input_t;

input_t *createInputElement(int x, int y, int width, int fontSize, uint8_t fg[3], uint8_t bg[3], int bpp){
	input_t *newInput = (input_t*)calloc(1,sizeof(input_t));
	newInput->visible = true;
	newInput->x = x;
	newInput->y = y;
	newInput->width = width;
	newInput->tempText = calloc(width+1,1);
	memset(newInput->tempText, 0x20, width);
	newInput->maxInputLength = 2;
	newInput->input = calloc(newInput->maxInputLength, 1);
	newInput->text = createTextElement(x, y, newInput->tempText, fontSize, fg, bg, bpp);
	return newInput;
}

void deleteInputElement(input_t *input){
	deleteTextElement(input->text);
	free(input->tempText);
	free(input->input);
	free(input);
	input = NULL;
}

void renderInput(input_t *input){
	memset(input->tempText, ' ', input->width+1);
	input->tempText[input->width] = 0;
	int l = strlen(input->input);
	strncpy(input->tempText, input->input, MIN(l, input->width));
	if(input->cursor < input->width && input->focused)
		input->tempText[input->cursor] = '|';
	setTextText(input->text, input->tempText);
	regenerateTextBuffer(input->text);
	renderText(input->text);
}

void backspaceInput(input_t *input){
	int a = 0;
	if(input->input[input->cursor] == 0){
		input->cursor--;
		a = 1;
	}
	input->input[input->cursor--] = 0;
	if(input->cursor < 0)
		input->cursor = a = 0;
	if(a)
		input->cursor++;
	renderInput(input);
}

void addCharacterToInput(unsigned char c, input_t *input){
	if(!c)
		return;
	switch(c){
		case 0x7f:
			backspaceInput(input);
			return;
			break;
		case '\n':
			if(input->onReturn)
				input->onReturn();
			return;
			break;
	}
	if(input->cursor+2 >= input->maxInputLength){
		input->maxInputLength*=2;
		input->input = realloc(input->input, input->maxInputLength);
	}
	input->input[input->cursor++] = c;
	input->input[input->cursor] = 0;
	if(input->onKeyPress)
		input->onKeyPress();
	renderInput(input);
}

#endif
