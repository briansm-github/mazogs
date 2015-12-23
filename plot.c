#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

#define SCALE 2 // screen size scaling. (usually between 1 and 5)

#define WIDTH  (128*SCALE)  // Arduboy 128x64 screen
#define HEIGHT (64*SCALE)

extern const unsigned char tiles24x16[];
extern const unsigned char tiles4x4[];
extern const  unsigned char  font[];

//--------------------------------------------------------
void display_display()
{
}
