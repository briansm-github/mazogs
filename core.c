#include <stdio.h>
#include <stdlib.h>

#include "defines.h"

extern unsigned char buffer[];
extern char zx81chars[];
extern char zx81chars_inv[];

// globals...
extern int read_maze();
extern int write_maze();
extern void black_screen();
extern void pause();
extern unsigned int random_seed();

extern int random8bit;

extern const unsigned char tiles24x16[];

unsigned char maze[1408]; // 64x44/2 (1 nibble per maze tile)

int  xfd;   // File Descriptor for the X display 
int pose; // character posture appearance
int frame=0; // global animation frame bit
int move_frame=0; // character movement frame bit
int carrying; // defines what player is carrying, e.g. sword
int level=1;
int moves_left;
int initial_moves;
int kill_moves;
unsigned int maze_number; //  which of the 65536 mazes to play.

//-----------------------------------------------------------
// write 'THIS WAY' tiles to map...
int thisway(int posn)
{
  clear_trails();
  if (carrying==HAVE_TREASURE) {
    write_maze(posn,TREASURE); // put missing treasure back in the map...
    solve_maze(HOME);
    write_maze(posn,TRAIL); // then revert back to trail
  } else solve_maze(posn);
  clear_badsearches();
  frame=50; // set countdown for thisway removal 
}

//---------------------------------------------------------------
// do the 16x16 map view in-game
int view_map(int posn)
{
  int x,y,i,tile1,tile2;
  
  // charge 10 moves..... (original says 15 for level2, source disagrees)
  if (level>1 && moves_left>11) moves_left-=10;
  grey_screen();
  for (i=25; i>0; i--) { // forced to view map for 25 frames
    for (y=0; y<16;y+=2) for (x=0; x<16; x++) {
      tile1=read_maze(posn+y*64+x-520);
      tile2=read_maze(posn+y*64+64+x-520);
      if (y==8 && x==8) tile1=MAP_STILL;
      draw4x4tile(tile1,tile2,x+8,y);
    }
    display_display();
    if (level==3) {
      move_mazogs(posn);
      if (read_maze(posn)==MAZOG) return(1); // fight!
    }
    pause(200);
  }
  return(0);
}
    
//----------------------------------------------------------
// routine to review entire map at the end of a game...
int explore_map()
{
   int x=0,y=0,i;
   int posn=816;
   int tile1,tile2;
   int keypress=1;
   
   
   while(read_keys()!=0); // wait for no keypresses
   for (y=0; y<16; y+=2) for (x=0; x<32; x++) {
        tile1=read_maze(posn+y*64+x);
	tile2=read_maze(posn+y*64+64+x);
        draw4x4tile(tile1,tile2,x,y);
      }
   print(0,24,"   USE D-PAD TO MOVE  ",1);
   print(0,32," PRESS A TO SOLVE     ",1);
   print(0,40," PRESS B FOR NEW GAME ",1);
   display_display();
   while(read_keys()==0); // wait for keypress
   while(read_keys()==KEY_B);
   while(1) {
      // draw map at current position...
      for (y=0; y<16; y+=2) for (x=0; x<32; x++) {
        tile1=read_maze(posn+y*64+x);
 	      tile2=read_maze(posn+y*64+64+x);
        draw4x4tile(tile1,tile2,x,y);        
      }
      display_display();
      pause(100);
      keypress=read_keys();
      if (keypress==KEY_UP && posn>63) posn-=64; 
      if (keypress==KEY_DOWN && posn<1984) posn+=64; 
      if (keypress==KEY_LEFT && posn>0) posn--; 
      if (keypress==KEY_RIGHT && posn<2048) posn++;
      if (keypress==KEY_A) {
        clear_trails();
        solve_maze(HOME);
	clear_badsearches();
      }
      if (keypress==KEY_B) { 
          while(read_keys());
          return(0);
      } 
    }
}

//-----------------------------------------------------------------
// Mazog move routines coming up....

int can_move(int posn)
{
   int tile;
   
   tile=read_maze(posn);
   if (tile==CLEAR || tile==TRAIL || tile==THISWAY) return(1);
   return(0);
}

//-----------------------------------------------------------------
int move_mazog(int posn, int newposn)
{
  write_maze(posn,CLEAR);
  write_maze(newposn,MAZOG);
}

//------------------------------------------------------------------
int move_mazogs(int posn)
{
  int i,dir;
  
  for (i=128; i<2944; i++)   // scan whole maze for mazogs
  
    if (read_maze(i)==MAZOG) {
       // always pick a fight if right or left of player...
       if (i==posn+1 || i==posn-1) {
         move_mazog(i,posn);
	        return(0); // start the fight.
       }
       dir=random_direction();
       
       // up.... (note that mazogs don't attack face-on)
       if (dir==1 && can_move(i-64) && i-64!=posn) move_mazog(i,i-64);
       
       // left.....
       if (dir==3 && can_move(i-1)) move_mazog(i,i-1); // left
       
       // now it gets tricky - avoid re-scanning same mazog twice
       
       // right....
       if (dir==4 && can_move(i+1)) {move_mazog(i,i+1); i++;} 
       
       // down.... (note that mazogs don't attack face-on)
       if (dir==2 && can_move(i+64) && i+64!=posn) {move_mazog(i,i+64); i+=64;}        
    }
  return(0);
}

//-----------------------------------------------------------------
// draw 5x4 big-tile maze at current position...
int draw_maze(int posn)
{
   int x,y,a,tile;
   
      for (y=0; y<4; y++) for (x=0; x<5; x++) {
	tile=read_maze(posn+y*64+x-130);
	if (x==2 && y==2) tile=pose; // put character in center of screen
	// this does the animations...
	if (frame&1 && tile==MAZOG) tile=MAZOG2;
	if (frame&1 && tile==TREASURE) tile=TREASURE2;
	if (frame&1 && tile==PRISONER) tile=PRISONER2;
	
        draw24x16tile(tile,x,y);
      }
      display_display();
}

//----------------------------------------------------------
// Process the next player move.
unsigned int check_move(int posn, int newposn)
{
  int tile;
  
  tile=read_maze(newposn);
  if (tile==WALL) { // can't walk though walls...
    pose=STILL+carrying; move_frame=0;
    return(posn);
  }
  else if (tile==SWORD) { // grab sword, or swap with treasure
     if (carrying==HAVE_NOTHING) write_maze(newposn,WALL); 
     else if (carrying==HAVE_TREASURE) write_maze(newposn,TREASURE); // swap
     carrying=HAVE_SWORD;
     pose=STILL+carrying; move_frame=0;
     return(posn);
  }
  else if (tile==PRISONER) { // get directions.
     if (level==3) write_maze(newposn,WALL);
     thisway(posn);
     return(posn); 
  }
  else if (tile==TREASURE) { // grab treasure, or swap with sword
     if (carrying==HAVE_NOTHING) write_maze(newposn,WALL);
     else if (carrying==HAVE_SWORD) write_maze(newposn,SWORD); // swap
     carrying=HAVE_TREASURE;
     pose=STILL+carrying; move_frame=0;
     return(posn); 
  }
  else if (tile==MAZOG) { // fight the mazog
    write_maze(posn,TRAIL); // leave a trail
    posn=newposn;
    if (fight(posn)) return(posn+32768); // fight sequence, exit if lost.
    if (carrying==HAVE_SWORD) carrying=HAVE_NOTHING;
  }
  
  // just movement then, first check there are enough moves left...
  if (level>1) {
    moves_left--; if (moves_left==0) // starved... (drop treasure)
     {if (carrying==HAVE_TREASURE) write_maze(posn,TREASURE); return(65535);} 
  }
  // now set pose for movement direction then....
  if (newposn-posn==-64) pose=UP+carrying+move_frame;
  if (newposn-posn==64) pose=DOWN+carrying+move_frame;
  if (newposn-posn==-1) pose=LEFT+carrying+move_frame;
  if (newposn-posn==1) pose=RIGHT+carrying+move_frame;
  // leave a trail...
  write_maze(posn,TRAIL);
  
  move_frame=!move_frame;
  return(newposn);
}
//----------------------------------------------------------
// fight routine. returns 0 for player win or 'posn' if lost.
int fight(int posn)
{
   int i,n;
   
   if (level>1) moves_left+=kill_moves;
   //printf("moves before=%i, after=%i\n",moves_left-kill_moves,moves_left);
   
   for (i=0; i<5; i++) // animation cut-scene thing
     for (n=0; n<6; n++) {
       // (avoid running down the 'thisway' timer...)
       if (n%2==0) { pose=FIGHT+n/2; frame=frame^1; }
       else pose=MAZOG;
       draw_maze(posn);
       pause(80);
     
   }
   write_maze(posn,CLEAR);
   if (carrying==HAVE_SWORD) {carrying=HAVE_NOTHING; pose=STILL; return(0);}
   // fighting without a sword... roll D20...
   update_random();
   if (random8bit>127) {pose=STILL; return(0);} // phew!
   return(posn); // RIP
}


//----------------------------------------------------------
// This is the main game-play loop. Exits at end of a game.
unsigned int enter_maze()
{
    unsigned int posn=HOME;
    int k;

    grey_screen();
    while(1) {
      if (level==3) move_mazogs(posn);
      if (read_maze(posn)==MAZOG) if (fight(posn)) return(posn); // fight result
      draw_maze(posn);
      pause(200);
      k=read_keys();
      if (k==KEY_UP) posn=check_move(posn,posn-64); 
      else if (k==KEY_DOWN) posn=check_move(posn,posn+64); 
      else if (k==KEY_LEFT) posn=check_move(posn,posn-1);        
      else if (k==KEY_RIGHT) posn=check_move(posn,posn+1); 
      else pose=STILL+carrying;
      
      if (posn==65535) return(65535); // starved.
      if (posn>32768) return(posn-32768); // lost a fight!

      if (k==KEY_B) situation_report(posn);
      if (k==KEY_A)
         if (view_map(posn)) // a fight started when viewing map
	         if (fight(posn)) return(posn); // check fight outcome.
      
      if (frame>1) {frame--; if (frame==1) clear_trails();} //remove thisway
      else frame=!frame;
      
      if (posn==HOME && carrying==HAVE_TREASURE) return(0); // a winner is you
    }
}

//----------------------------------------------------------
void setup_c()
{
  intro();
  LOAD(); // nostalgia  
}

//----------------------------------------------------------
void loop_c()
{
  unsigned int end_posn;

  
  initial_moves=0; moves_left=0; carrying=HAVE_NOTHING; pose=STILL;
  title_sequence();
  
  choose_level();
  maze_number=true_random(); // seed RNG
  level_splash();
   
  left_or_right();
  situation_report(HOME);
  if (level>1) situation_report2();
    
  end_posn=enter_maze(); // begin the game, process outcome...
  if (end_posn==65535) starved(); // starved();
  else if (end_posn!=0) mazogs_win(end_posn);
  else welcome_back();
  maybe_examine_maze();
  
}

