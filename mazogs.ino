// MAZOGS - A MAZE ADVENTURE GAME
// Ported from the ZX81 '82 code to Arduboy over a rainy November '15 week.
// Original version by Don Priestley. Bonsai porting by Brian Smith.

#include <SPI.h>
#include "Arduboy.h"
#include "defines.h"

Arduboy Arduboy;
long ltime;

extern "C" void setup_c();
extern "C" void loop_c();
extern "C" const  unsigned char tiles24x16[];
extern "C" const  unsigned char tiles4x4[];
extern "C" const  unsigned char font[];

//------------------------------------------------------------
extern "C" int read_keys();
int read_keys() // swap 'KEY_A' and 'KEY_B' around to suit taste
{
  uint8_t input = Arduboy.getInput();
  if (input & (1<<5)) return(KEY_LEFT);
  if (input & (1<<4)) return(KEY_UP);
  if (input & (1<<3)) return(KEY_RIGHT);
  if (input & (1<<2)) return(KEY_DOWN);
  if (input & (1<<0)) return(KEY_A);  // a button?
  if (input & (1<<1)) return(KEY_B);  // b button?
  return(0);  
}

//------------------------------------------------------------
extern "C" void pause(int d); // pause for 'd' milliseconds
void pause(int d)
{
 delay(d);
}

//------------------------------------------------------------
extern "C" unsigned int true_random();
unsigned int true_random()
{
  return(micros()%65536); // use microsecond counter for entropy
}
//------------------------------------------------------------

extern "C" void seed_pseudo_random(unsigned int r);
void seed_pseudo_random(unsigned int r)
{
   randomSeed(r);
}

//------------------------------------------------------------

extern "C" unsigned int pseudo_random();
unsigned int pseudo_random()
{
   return(random(65536));
}

//------------------------------------------------------------
extern "C" void black_screen();
void black_screen()
{
  Arduboy.blank();
}

//------------------------------------------------------------
extern "C" void white_screen();
void white_screen()
{
  Arduboy.white();
}

//------------------------------------------------------------
extern "C" void grey_screen();
void grey_screen()
{
  Arduboy.grey();
}

//------------------------------------------------------------
extern "C" void draw_bitmap(unsigned char *bitmap, int posn, int size, int invert);
void draw_bitmap(unsigned char *bitmap, int posn, int size, int invert)
{
  Arduboy.drawbitmap(bitmap,posn,size,invert);
}

//--------------------------------------------------------
// draw big tile, x=0-4, y=0-3.
extern "C" void draw24x16tile(int tile,int x, int y);
void draw24x16tile(int tile,int x, int y)
{
  Arduboy.draw24x16tile(&tiles24x16[tile*24*2],4+x*24,y*2);
}

//--------------------------------------------------------
// this takes 2 stacked tiles to draw (8 bit depth)
// x=0-31, y=0-14 (divisor of 2)
extern "C" void draw4x4tile(int tile1,int tile2, int x, int y);
void draw4x4tile(int tile1,int tile2, int x, int y)
{ 
  int i,z;
  unsigned char a[4];
  
  
  for (i=0; i<4; i++) {
    a[i]=pgm_read_byte(tiles4x4+tile2*4+i)*16+pgm_read_byte(tiles4x4+tile1*4+i);
  };
  Arduboy.draw4x4tiles(a,x*4,y/2);
}

//------------------------------------------------------------
// prints text at location x,y. y is usually a multiple of 8.
extern "C" void print(int x, int y, char *s, int colour);
void print(int x, int y, char *s, int colour)
{
  unsigned char c,c6[6];
  int i;
  
  c=*s++; 
  while(c!=0) {
    if (colour==1) {
        for (i=0; i<5; i++) c6[i]=pgm_read_byte(font+c*5+i); c6[5]=0;
        Arduboy.drawchar(c6,x,y/8);
    } else {
         for (i=0; i<5; i++) c6[i]=~pgm_read_byte(font+c*5+i); c6[5]=255;
         Arduboy.drawchar(c6,x,y/8);
    }
    c=*s++; // next character in the string
    x+=6;
  }
}

//-------------------------------------------------------------

void setup() {
  
  SPI.begin();
  Arduboy.start();
  Arduboy.blank();
  ltime = millis();

  setup_c();
}

void loop() {

   loop_c();
}
