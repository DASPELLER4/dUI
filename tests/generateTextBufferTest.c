#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/dui.h"

int main(int argc, char **argv){
        int bpp = 1;
        text_t *text = createTextElement(0, 0, "jq", TEXT_S, WHITE, BLACK, bpp);
        renderText(text);
        for(int j = 0; j < 8*text->fontSize; j++){
                for(int i = text->byteWidth*j; i < text->byteWidth*(j+1); i++){
                        if(text->textbuffer[i]>0)
                                printf("#");
                        else
                                printf(" ");
                }
                printf("\n");
        }
        deleteTextElement(text);
}
