#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

typedef struct{
	int fd;
	unsigned char keypressed;
	struct termios orig;
} keyboard_t;

keyboard_t *getKeyboard(){
	keyboard_t *newKeyboard = calloc(1, sizeof(keyboard_t));
	newKeyboard->fd = STDIN_FILENO;
	fcntl(newKeyboard->fd, F_SETFL, fcntl(newKeyboard->fd, F_GETFL)|O_NONBLOCK);
	tcgetattr(newKeyboard->fd, &newKeyboard->orig);
	struct termios raw = newKeyboard->orig;
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(newKeyboard->fd, TCSAFLUSH, &raw);
	return newKeyboard;
}

void closeKeyboard(keyboard_t *keyboard){
	tcsetattr(keyboard->fd, TCSAFLUSH, &keyboard->orig);
	fcntl(keyboard->fd, F_SETFL, fcntl(keyboard->fd, F_GETFL)^O_NONBLOCK);
	free(keyboard);
	keyboard = NULL;
}

void updateKeyboard(keyboard_t *keyboard){
	if(read(keyboard->fd, &keyboard->keypressed, 1)<=0)
		keyboard->keypressed = 0;
}

#endif

