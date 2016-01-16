#include <stdio.h>
#include "defines.h"

extern unsigned char zx81chars[];
extern unsigned char buffer[];

extern void pause();

//----------------------------------------------------------
// I remember when this was all fields....
int LOAD()
{
    int x,y,i; 
    
    white_screen(); 
    display_display(); pause(500);
    
    draw_bitmap(&zx81chars[384],776,8,1); // inv-K
    display_display(); pause(500);
    
    draw_bitmap(&zx81chars[392],776,8,0); // L
    draw_bitmap(&zx81chars[416],784,8,0); // O
    draw_bitmap(&zx81chars[304],792,8,0); // A
    draw_bitmap(&zx81chars[328],800,8,0); // D
    draw_bitmap(&zx81chars[392],818,8,1); // inv-L
    display_display(); pause(500);
    
    draw_bitmap(&zx81chars[88],816,8,0); // "
    draw_bitmap(&zx81chars[392],824,8,1); // inv-L
    display_display(); pause(300);

    draw_bitmap(&zx81chars[88],824,8,0); // "
    draw_bitmap(&zx81chars[392],832,8,1); // inv-L
    display_display(); pause(500);
  
    white_screen(); 
    display_display(); pause(1000);
    return(0);
}
