#ifndef MOUSE_H
#define MOUSE_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define MICE "/dev/input/mice"
#define SENSITIVITY 2

typedef struct{
	int x;
	int y;
	char clicked;
	char justClicked;
	int fd;
	char color[3];
	int size;
} mouse_t;

mouse_t *getMouse(){
	mouse_t *newMouse = calloc(1,sizeof(mouse_t));
	if((newMouse->fd = open(MICE, O_RDONLY | O_NONBLOCK)) == -1){
		free(newMouse);
		return NULL;
	}
	newMouse->color[0] = 0xFF;
	newMouse->color[1] = 0xFF;
	newMouse->color[2] = 0xFF;
	newMouse->size = 8;
	newMouse->x = newMouse->y = 0;
	newMouse->clicked = 0;
	newMouse->justClicked = 0;
	return newMouse;
}

void updateMouse(mouse_t *mouse){
	char data[3];
	int s = 0;
	if((s = read(mouse->fd, data, 3)) <= 0){
		mouse->justClicked = 0;
		return;
	}
	char prevClick = mouse->clicked;
	mouse->clicked = data[0]&0x1;
	mouse->justClicked = (~prevClick&0x1)&mouse->clicked;
	mouse->x += data[1]*SENSITIVITY;
	mouse->y -= data[2]*SENSITIVITY;
}

void closeMouse(mouse_t *mouse){
	close(mouse->fd);
	free(mouse);
	mouse = NULL;
}

#endif

