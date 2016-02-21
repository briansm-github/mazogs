#include <stdio.h>
#include <avr/pgmspace.h>
#include "defines.h"


extern int level,carrying,initial_moves,moves_left,kill_moves,frame,pose;
extern unsigned int maze_number;
extern void black_screen();
extern void pause();

PROGMEM const unsigned char mazogs_bmp[]={
0x0,0x7e,0x2,0x1e,0x2,0x7e,0x0,0x7e,0x12,0x7e,0x0,0x72,
0x4a,0x46,0x0,0x7e,0x42,0x7e,0x0,0x7e,0x42,0x72,0x0,0x4e,0x4a,
0x7a,0x0,0x0,0x0,0x0,0x0,0x0};

// intro
void intro()
{
  black_screen();
  print(42,24,"ARDUBOY",1);
  display_display();
  pause(1500);
}

//----------------------------------------------------------
// runs the 'attract' opening screen
int title_sequence()
{
  int i,x,y,frame=0;

  while(read_keys()); // wait till no keys pressed...
  // static screen elements...
  grey_screen();
  for (y=1; y<4; y++) for (x=0; x<5; x++) draw24x16tile(WALL,x,y);
  draw24x16tile(STILL+HAVE_TREASURE,0,1);
  draw24x16tile(TRAIL,0,2);
  draw24x16tile(TRAIL,1,2);
  draw24x16tile(RIGHT+HAVE_SWORD,2,2);
  draw24x16tile(THISWAY,1,3);
  draw24x16tile(SWORD,3,3);
 
  get_b: 
  draw_bitmap(mazogs_bmp, 256+frame%6*128+100,28,frame%12<6);
  if (frame%20==0) {print(1,0,"A MAZE ADVENTURE GAME",0);};
  if (frame%20==10) {print(1,0," PRESS 'B' TO START  ",1);};
  // animation loop...
  if (frame&1) {
    draw24x16tile(TREASURE2,1,1);
    draw24x16tile(MAZOG2,3,2);
    draw24x16tile(PRISONER,2,3);
  } else {
    draw24x16tile(TREASURE,1,1);
    draw24x16tile(MAZOG,3,2);
    draw24x16tile(PRISONER2,2,3);  
  }
  frame++;
  display_display();
  for (i=0; i<150; i++) {
    if (read_keys()==KEY_B) return(0);
    pause(2);
  }
  goto get_b;  
}

//----------------------------------------------------------
int choose_level()
{
  int x,y,i;
  
  while(read_keys());
  
  black_screen();
  for (y=1; y<4; y++) for (x=0; x<5; x++) draw24x16tile(WALL,x,y);
  draw24x16tile(MAZOG,2,1);
  print(20,0," WHICH GAME ? ",0);
  display_display();

  choose_loop:
  i=read_keys();
  if ((i==KEY_UP || i==KEY_LEFT) && level>1) level--;  // up
  if ((i==KEY_DOWN || i==KEY_RIGHT) && level<3) level++;  // down
  
  if (level==1) print(7,48,"    TRY IT OUT     ",1);
  if (level==2) print(7,48,"  FACE A CHALLENGE ",1);
  if (level==3) print(7,48,"MANIC MOBILE MAZOGS",1);
  if (i==KEY_B) return(0);  // B key
  display_display();
  
  while(read_keys());
  while(!read_keys());
  
  goto choose_loop;     
}

//----------------------------------------------------------
int level_splash()
{
  int x,y,i;
  
  black_screen;
  while(read_keys());
  
  for (y=1; y<4; y++) for (x=0; x<5; x++) draw24x16tile(WALL,x,y);
  
  if (level==1) {
    draw24x16tile(LEFT+HAVE_TREASURE,2,1);
    print(18,0,"  TRY IT OUT   ",0);
  }
  if (level==2) {  
    draw24x16tile(RIGHT+HAVE_SWORD,2,1);
    print(10,0," FACE A CHALLENGE ",0);
  }
  if (level==3) {
    draw24x16tile(STILL,2,1);
    print(1,0," MANIC MOBILE MAZOGS ",0);
    for (i=1; i<6; i++) {
      draw24x16tile(MAZOG2,1,1);
      draw24x16tile(MAZOG2,3,1);
      display_display();
      pause(250);
      draw24x16tile(MAZOG,1,1);
      draw24x16tile(MAZOG,3,1);
      display_display();
      pause(250);
    }
  }
  // print instructions...
  print(0,48," MAZE IS BEING DRAWN ",1);
  display_display();
  create_maze();
  
  pause(500); // remove if too slow...
  print(0,48,"MAZE READY, PRESS 'B'",1);
  display_display(); 
  while(read_keys()!=KEY_B);    
}

//----------------------------------------------------------
int p10(int x) {
if (x==0) return(10000);
if (x==1) return(1000);
if (x==2) return(100);
if (x==3) return(10);
if (x==4) return(1);
}

void pick_maze()
{
  char s[5];
  int i,p=4;
   
  grey_screen();
  print(0,0,"     MAZE CHOOSER    ",1);
  print(0,8,"PRESS B WHEN FINISHED",0);
  loop: 
  sprintf(s,"%.5u",maze_number);
  print(48,40,s,0);
  sprintf(s,"     "); s[p]='V';
  print(48,32,s,0);
  display_display();
  i=read_keys();
  if (i==KEY_LEFT && p>0) p--;
  if (i==KEY_RIGHT && p<4) p++;
  if (i==KEY_UP && maze_number<=65535-p10(p)) maze_number+=p10(p);
  if (i==KEY_DOWN && maze_number>p10(p)) maze_number-=p10(p);
  if (i==KEY_B) goto exit;
  while(read_keys());
  goto loop;

  exit:  
  print(0,56,"REDRAWING MAZE...",0);
  create_maze();
}

//-----------------------------------------------------------
int left_or_right()
{
  int x,y,tile1,tile2,distance;

  start:
  grey_screen();
  for (y=0; y<16;y+=2) for (x=0; x<16; x++) {
    tile1=read_maze(HOME+y*64+x-520);
    tile2=read_maze(HOME+(y+1)*64+x-520);
    draw4x4tile(tile1,tile2,x,y);
  }
  print(68,24,"WHICH WAY?",0);
  print(68,32,"LEFT OR   ",0);
  print(68,40,"RIGHT?    ",0);
  display_display();
  
  while(read_keys());
 
  leftright:
  x=read_keys();
  if (x==KEY_LEFT) {write_maze(HOME+1,WALL); goto chosen; } // left
  if (x==KEY_RIGHT) {write_maze(HOME-1,WALL); goto chosen; } // right
  if (x==KEY_A) {pick_maze(); goto start;}
  goto leftright;
  chosen:
  grey_screen();
  
  display_display();
  
  pause(100);
  for (y=0; y<16;y+=2) for (x=0; x<16; x++) {
    tile1=read_maze(HOME+y*64+x-520);
    tile2=read_maze(HOME+(y+1)*64+x-520);
    draw4x4tile(tile1,tile2,x,y);  
  }
  
  check_loop:
  solve_maze(HOME);
  distance=get_distance();
  if (distance<100) { // the game would be too easy...
    move_treasure();
    goto check_loop;
  }
  clear_trails();
  
  print(68,24,"PRESS 'B' ",0);
  print(68,32,"FOR REPORT",0);
  display_display();
  while(read_keys()!=KEY_B);
}

//----------------------------------------------------------
int situation_report(int posn)
{
  int i;
  unsigned char s[10];
  
  grey_screen();
  print(16,0,"SITUATION REPORT",1);
  display_display();
  if (carrying==HAVE_TREASURE) {
    write_maze(posn,TREASURE);
    solve_maze(HOME);
    write_maze(posn,CLEAR);
    print(0,16,"MOVES BACK TO \"BASE\"=",0);
    i=get_distance();
    sprintf(s," %i ",i);
    print(50,24,s,0);
    print(10,56,"PRESS 'B' FOR GAME",0);   
  } else {
    solve_maze(posn);
    print(10,16,"MOVES TO TREASURE=",0);
    i=get_distance();
    sprintf(s," %i ",i);
    print(50,24,s,0);
    if (moves_left==0) { // i.e. first report of game...
       moves_left=i*4;
       initial_moves=moves_left;
       if (level>1) print(10,56,"PRESS 'B' FOR MORE",0); // second report due....
       else print(10,56,"PRESS 'B' FOR GAME",0);
    }
    else print(10,56,"PRESS 'B' FOR GAME",0);
  }
  if (level>1) {
      print(16,32,"MOVES LEFT=",0);
      sprintf(s,"%i ",moves_left);
      print(82,32,s,0);
      if (carrying==HAVE_NOTHING) {
        print(16,40,"'A' = BUY SWORD",0);
      }
  }
  clear_trails();
  display_display();
  
  // charge 10 moves.....
  if (level>1 && moves_left>11) moves_left-=10;
  
  while(read_keys());
  
  get_ab:
  // 'A' key, buy a sword? Costs half of remaining moves :o
  if (read_keys()==KEY_A && level>1 && carrying==HAVE_NOTHING) {
    carrying=HAVE_SWORD;
    moves_left=moves_left/2+1;
    while(read_keys()==KEY_A);
    grey_screen();
    return(0); 
  }
  if (read_keys()==KEY_B) {grey_screen(); return(0);}
  goto get_ab;  
}

//----------------------------------------------------------
int situation_report2()
{
  int i;
  unsigned char s[10];
  
  grey_screen();
  print(16,0,"SITUATION REPORT",1);
  display_display();
  
  if (level==2) kill_moves=initial_moves/10;
  if (level==3) kill_moves=initial_moves*15/100;
  print(0,16,"EACH KILL=+",0);
  sprintf(s,"%i MOVES ",kill_moves); print(66,16,s,0);
  print(0,24,"EACH VIEW=-10 MOVES  ",0);
  print(0,32,"EACH REPORT=-10 MOVES",0);
 
  print(10,55,"PRESS 'B' FOR GAME",0);
  display_display();

  while(read_keys());
  
  while(read_keys()!=KEY_B);
  return(0);  // B key
}

//-----------------------------------------------------------
void starved()
{
  int i;
  
  black_screen();
  // cursor set to center
  frame=0;
  for (i=0; i<20; i++) {
     print(16,24,"YOU HAVE STARVED",frame);
     print(16,32,"    TO DEATH    ",frame);
     display_display();
     frame=!frame;
     pause(200);
  }
}

//---------------------------------------------------------
void mazogs_win(int posn)
{
  int i;
  
  pose=MAZOG; write_maze(posn,MAZOG);
  frame=0;
  for (i=0; i<20; i++) {
    draw_maze(posn);
    frame=!frame;
    pause(100);
  }
  black_screen(); display_display();
  pause(100);
  for (i=0; i<20; i++) {
   draw_maze(posn);
   // cursor set
   print(14,48,"  DEATH TO ALL  ",frame);
   print(14,56,"TREASURE SEEKERS",frame);
   display_display();
   frame=!frame;
   pause(200);
  } 
  grey_screen();
}

//----------------------------------------------------------
void welcome_back()
{
  int i,posn;
  char s[10];
  
  if (read_maze(HOME-1)==WALL) posn=HOME+1; else posn=HOME-1;
  
  write_maze(HOME-64,EXIT);
  write_maze(HOME,STILL); // reception party.
  
  pose=STILL+HAVE_TREASURE;
  
  for (i=0; i<30; i++) {
    draw_maze(posn);
    print(20,56," WELCOME BACK ",frame);
    display_display();
    frame=!frame;
    pause(200);
  }
  write_maze(HOME,MAP_STILL);
  
  grey_screen();
  if (level>1) { // report the score.
    print(12,8,"MOVES ALLOWED=",0);
    sprintf(s,"%i",initial_moves);
    print(96,8,s,0);
    print(12,16,"MOVES LEFT=",0);
    sprintf(s,"%i",moves_left);
    print(78,16,s,0);
    print(12,24,"SCORE=",0);
    // prevent 16-bit overflow in score....
    initial_moves/=10;
    sprintf(s,"%i%%",moves_left*10/initial_moves);
    print(48,24,s,0);
  }
}

//----------------------------------------------------------
int maybe_examine_maze()
{
  // cursor set to bottom of screen 
  print(20,48,"A=EXAMINE MAZE",0);
  print(20,56,"B=ANOTHER GAME",0);
  display_display();
  get_ab:
  if (read_keys()==KEY_A) {explore_map(); return(0);}
  if (read_keys()==KEY_B) return(0);
  goto get_ab;  
}

