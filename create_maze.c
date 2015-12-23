#include <stdio.h>
#include <stdlib.h>
#include "defines.h"

extern unsigned char maze[]; // 64x44/2
extern unsigned int maze_number;

// globals
int random8bit=195;
int counter=1;
int counter2=70;


//-------------------------------------------------------------
// write to a maze location...
// each maze tile stored in 4 bits, 2 positions per byte.
// (This would be a lot simpler and faster if the Arduboy had more
// than 3K of RAM. )

int write_maze(int posn, int value)
{
  if (posn<128) return(0); // can't write to maze boundary.
  if (posn>=2944) return(0); // ditto
  if (posn&1) {maze[posn/2-64]&=15; maze[posn/2-64]|=value*16; }
  else {maze[posn/2-64]&=240; maze[posn/2-64]|=value; }
  
  // maze[posn]=value; // simpler version with byte-per-tile
}


//------------------------------------------------------------
// read from a maze location...
// (again, tricks required to fit maze into <1.5K of RAM)

int read_maze(int posn)
{
  if (posn<64) return(WALL_GREY);
  if (posn<128) return(WALL);
  if (posn>=3008) return(WALL_GREY);
  if (posn>=2944) return(WALL); // maze boundaries
  if (posn&1) return(maze[posn/2-64]/16);
  else return(maze[posn/2-64]&15);
  
  // return(maze[posn]); // simpler version with byte-per-tile
}

//------------------------------------------------------------
// create blank maze template...
void blank_maze()
{
  int i;
    
  for (i=0; i<3072; i++) {
    if (i<64 || i>=3008) write_maze(i,WALL_GREY);
    else if (i%64==0 || i%64==63) write_maze(i,WALL_GREY);
    else write_maze(i,WALL);
  }
}

//-------------------------------------------------------------
// add treasure to maze...
// 
int add_treasure()
{
  int i;
  
  treasure_pick:
  i=(rand()%2810)+128; // use the arduboy random seed!
  if (read_maze(i)==WALL_GREY || read_maze(i+1)==WALL_GREY ||
      read_maze(i-1)==WALL_GREY) goto treasure_pick;
  write_maze(i,TREASURE);
  return(i);
}

//-------------------------------------------------------------
// move treasure in maze (if solution is not long enough)
int move_treasure()
{
  int posn=0,i;
  
  // first, find the treasure!
  for (i=128; i<128+2810; i++) if (read_maze(i)==TREASURE) posn=i;
  write_maze(posn,CLEAR); // remove old treasure location...
  add_treasure();
}

//-------------------------------------------------------------
// pseudorandom 8-bit number generator.
// x(n+1) = ( x(n)*67+509 ) modulo 256.
// (note that this has a period of 128 instead of 256.)
void update_random()
{
  random8bit=(random8bit*67+509)%256;
  //printf("random now %i\n",random8bit);
}

//-------------------------------------------------------------
// just set 'random8bit' to a new random number, probably
// from the program or frame counter or something.
void seed_random8bit()
{
  random8bit=rand()%256;
  update_random();
}


//-------------------------------------------------------------
// decode random8bit to a direction (1/2/3/4)
int random_direction()
{
  update_random(); //  printf("new random=%i\n",random8bit);
  if (random8bit<64) return(1);
  if (random8bit<128) return(2);
  if (random8bit<192) return(3);
  return(4);
}

//----------------------------------------------------
// try_left() - returns 1 if possible to move left.
int try_left(int posn)
{
  if (read_maze(posn-1)!=WALL) return(0);
  if (read_maze(posn-2)!=WALL) return(0);
  if (read_maze(posn+63)!=WALL) return(0);
  if (read_maze(posn-65)!=WALL) return(0);
  
  //printf("left\n");
  return(1); 
}

// try_right() - returns 1 if possible to move right.
int try_right(int posn)
{
  if (read_maze(posn+1)!=WALL) return(0);
  if (read_maze(posn+2)!=WALL) return(0);
  if (read_maze(posn+65)!=WALL) return(0);
  if (read_maze(posn-63)!=WALL) return(0);
  
  //printf("right\n");
  return(1); 
}

// try_up() - returns 1 if possible to move up.
int try_up(int posn)
{
  if (read_maze(posn-65)!=WALL) return(0);
  if (read_maze(posn-64)!=WALL) return(0);
  if (read_maze(posn-63)!=WALL) return(0);
  if (read_maze(posn-128)!=WALL) return(0);  
  //printf("up\n");
  return(1); 
}


// try_down() - returns 1 if possible to move down.
int try_down(int posn)
{
  if (read_maze(posn+63)!=WALL) return(0);
  if (read_maze(posn+64)!=WALL) return(0);
  if (read_maze(posn+65)!=WALL) return(0);
  if (read_maze(posn+128)!=WALL) return(0);  
  //printf("down\n");
  return(1); 
}

//-------------------------------------------------------------
// add_pathways - generate the maze pathways.
// starts from the given treasure location.
int add_pathways(int treasure)
{
  int timeout=65535;
  int posn=treasure;
  int newposn;
  int dir;
  int b;
  int size;
  int i;
  
  mloop:
  dir=random_direction();
  if (dir==2) goto try_right;
  if (dir==3) goto try_up;
  if (dir==4) goto try_down;
  if (try_left(posn)) {posn--; write_maze(posn,CLEAR); goto mloop;}
  try_right:
  if (try_right(posn)) {posn++; write_maze(posn,CLEAR); goto mloop;}
  try_up:
  if (try_up(posn)) {posn-=64; write_maze(posn,CLEAR); goto mloop;}
  try_down:
  if (try_down(posn)) {posn+=64;  write_maze(posn,CLEAR); goto mloop;}
  if (try_left(posn)) {posn--; write_maze(posn,CLEAR); goto mloop;}
  if (try_right(posn))  {posn++; write_maze(posn,CLEAR); goto mloop;}
  if (try_up(posn)) {posn-=64; write_maze(posn,CLEAR); goto mloop;}
    
  // the current path has ended, try to start a new one...
  find_new:
  timeout--; if (timeout==0) goto exit;
  counter=(counter+1)%256;
  if (counter==0) seed_random8bit();
  update_random();
  posn=random8bit*11+128;
  if (read_maze(posn)==CLEAR) goto mloop;
  for (b=0; b<6; b++) {posn++; if (read_maze(posn)==CLEAR) goto mloop;}
  goto find_new;
 
  // finally, get the size of the maze as a return value...
  exit:
  size=0;
  for (i=0; i<3072; i++) if (read_maze(i)==CLEAR) size++;
  //printf("maze size=%i\n",size);
  return(size);  
}

//------------------------------------------------------------------------
// add the maze entrance and check it creates a valid maze.
// takes the start location in the maze as argument.
int add_entrance()
{
  int posn;
    
  write_maze(1279,WALL);  
  write_maze(1280,SWORD);
  write_maze(1407,WALL);
  write_maze(1408,WALL);
  
  // start location in maze saved as 1344...
  posn=1344;
 
  left_check:
  write_maze(posn,CLEAR);
  if (read_maze(posn-1)==CLEAR) goto left_ok;
  if (read_maze(posn-65)==CLEAR) {write_maze(posn-64,CLEAR); goto left_ok;}
  if (read_maze(posn+63)==CLEAR) {write_maze(posn+64,CLEAR); goto left_ok;}
  posn--;
  goto left_check;
  
  left_ok:
  posn=1344;
  write_maze(1344,CLEAR); // redundant?
  
  right_check:
  write_maze(posn,CLEAR);

  if (read_maze(posn+1)==CLEAR) goto right_ok;
  if (read_maze(posn-63)==CLEAR) {write_maze(posn-64,CLEAR); goto right_ok;}
  if (read_maze(posn+65)==CLEAR) {write_maze(posn+64,CLEAR); goto right_ok;}  
  posn++;
  goto right_check;
  
  right_ok:
  write_maze(posn,CLEAR); // redundant?
}

//------------------------------------------------------------------------
// find a random wall in maze that's good for a sword or prisoner...
int find_good_wall_spot()
{
  int posn,timer=10000;
  
  wspot_test:
  timer--; if (timer==0) return(0);  // safety
  
  counter2++;  if (counter2==256) counter2=0;
  if (counter2==173) seed_random8bit();
  update_random();
  posn=random8bit*11+128;
  if (read_maze(posn)!=WALL) goto wspot_test;
  if (read_maze(posn-1)==WALL_GREY) goto wspot_test;
  if (read_maze(posn+1)==WALL_GREY) goto wspot_test;
  return(posn);
}

//------------------------------------------------------------------------
// find a random clearing in maze that's good for a mazog...
int find_good_clear_spot()
{
  int posn,timer=10000;
  
  cspot_test:
  timer--; if (timer==0) return(0); // safety
  
  counter2++;   if (counter2==256) counter2=0;
  if (counter2==173) seed_random8bit();
  update_random();
  posn=random8bit*11+128;
  if (read_maze(posn)!=CLEAR) goto cspot_test;
  if (read_maze(posn-1)==WALL_GREY) goto cspot_test;
  if (read_maze(posn+1)==WALL_GREY) goto cspot_test;
  return(posn);
}

//------------------------------------------------------------------------
int add_swords(int swords_count)
{
  int i,posn;
  
  for (i=0; i<swords_count; i++) {
    posn=find_good_wall_spot();
    if (posn==0) return(1);
    //printf("placing sword at %i\n",posn);
    write_maze(posn,SWORD);
  }
  return(0); // OK
}

//------------------------------------------------------------------------
int add_prisoners(int prisoners_count)
{
  int i,posn;
  
  for (i=0; i<prisoners_count; i++) {
    posn=find_good_wall_spot();
    if (posn==0) return(1);
    //printf("placing prisoner at %i\n",posn);
    write_maze(posn,PRISONER);
  }
  return(0); // OK
}

//------------------------------------------------------------------------
int add_mazogs(int mazogs_count)
{
  int i,posn;
  
  for (i=0; i<mazogs_count; i++) {
    posn=find_good_clear_spot();
    if (posn==0) return(1);
    //printf("placing mazog at %i\n",posn);
    write_maze(posn,MAZOG);
  }
  return(0); // OK
}

//-------------------------------------------------------------------------
// maze creator - runs all the sub-tasks.
void create_maze()
{
    int treasure_location;
    int maze_size;
    
    srandom(maze_number ); // seed RNG  
    random8bit=195; counter=1; counter2=70; // ensure consistancy
    
    mstart: 
    blank_maze();
    treasure_location=add_treasure();
    maze_size=add_pathways(treasure_location);
    if (maze_size<1201) {
       //printf("the maze is being redrawn.....\n");
       goto mstart; // check maze is complex enough
    }    
    add_entrance();
    seed_random8bit();
    if (add_swords(40)) goto mstart;
    if (add_prisoners(30)) goto mstart;
    if (add_mazogs(38)) goto mstart;
}

