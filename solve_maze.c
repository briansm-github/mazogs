#include <stdio.h>
#include "defines.h"

extern int read_maze();
extern int write_maze();
extern int carrying;

//--------------------------------------------------------
// remove trails.
clear_trails()
{
  int i,tile;
  
  for (i=128; i<2944; i++) {
    tile=read_maze(i);
    if (tile==TRAIL || tile==THISWAY) write_maze(i,CLEAR);
    //if (a==MAZOG) write_maze(i,CLEAR); // temporary hack!
  }
}

//----------------------------------------------------------

clear_badsearches()
{
  int i,a;
  
  for (i=128; i<2944; i++) {
    a=read_maze(i);
    if (a==NOTTHISWAY) write_maze(i,CLEAR);
    if (a==NOTTHISWAY_MAZOG) write_maze(i,MAZOG);
    if (a==THISWAY_MAZOG) write_maze(i,MAZOG);   
  }
}
//--------------------------------------------------------
// solve maze from given starting position to the treasure.
int solve_maze(int start)
{
  int posn;
  int tile;
 
  // first, clear the maze of any previous trails...
  clear_trails();
  posn=start;
  search:
    tile=read_maze(posn);
    if (tile==CLEAR) write_maze(posn,THISWAY);
    if (tile==MAZOG) write_maze(posn,THISWAY_MAZOG);
		 
    // first, see if we've finished the search...
    if (read_maze(posn+1)==TREASURE) goto found;
    if (read_maze(posn-1)==TREASURE) goto found;
    if (read_maze(posn+64)==TREASURE) goto found;
    if (read_maze(posn-64)==TREASURE) goto found;
    
  find_clear:
    tile=read_maze(posn+1); // right...
    if (tile==CLEAR || tile==MAZOG) {posn+=1; goto search;}
    tile=read_maze(posn-1); // left...
    if (tile==CLEAR || tile==MAZOG) {posn-=1; goto search;}
    tile=read_maze(posn+64); // down...
    if (tile==CLEAR || tile==MAZOG) {posn+=64; goto search;}
    tile=read_maze(posn-64); // up...
    if (tile==CLEAR || tile==MAZOG) {posn-=64; goto search;}
    
    // oops! time to backtrack.....
    tile=read_maze(posn);
    if (tile==THISWAY) write_maze(posn,NOTTHISWAY);
    if (tile==THISWAY_MAZOG) write_maze(posn,NOTTHISWAY_MAZOG);
    tile=read_maze(posn+1); // right...
    if (tile==THISWAY || tile==THISWAY_MAZOG) {posn+=1; goto find_clear;}
    tile=read_maze(posn-1); // left...
    if (tile==THISWAY || tile==THISWAY_MAZOG) {posn-=1; goto find_clear;}
    tile=read_maze(posn+64); // down...
    if (tile==THISWAY || tile==THISWAY_MAZOG) {posn+=64; goto find_clear;}
    tile=read_maze(posn-64); // up...
    if (tile==THISWAY || tile==THISWAY_MAZOG) {posn-=64; goto find_clear;}
    found:
    return(0);
}

//---------------------------------------------------------
// get_distance - returns count of 'this way' tiles.
int get_distance()
{
  int c=0;
  int i;
  int tile;
  
  for (i=128; i<2944; i++) {
   tile=read_maze(i);
   if (tile==THISWAY || tile==THISWAY_MAZOG) c++;
  }
  clear_badsearches();
  return(c);
}
