# mazogs
Arduboy / Linux port of ZX81 Mazogs game

This is as accurate a copy of the original ZX81 game as I can manage within
the confines of the Arduboy hardware. As with the original there is no sound as the ZX81 hardware does not support it. Maze generation logic etc are similar.

Use Arduino IDE version 1.6.6 or newer to compile. You will probably get an warning about "Low memory available, stability problems may occur' - you can safely ignore this, in practive the game runs stable. 

The original ZX81 version instructions etc apply and are at:
http://www.zx81stuff.org.uk/zx81/tape/Mazogs

The file 'graphics.zip' contains the MS Paint BMP files used and a C program
to generate the tiles4x4.c and tiles24x16.c files.

The file 'linux.zip' contains a linux port of the game, used for development.

