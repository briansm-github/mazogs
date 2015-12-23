OBJS  =  go.o plot.o create_maze.o solve_maze.o text_bits.o misc.o charset.c

CC=gcc 

CFLAGS= -O2
LDFLAGS=  -O2 -Wall -I/usr/include/X11 -L/usr/X11R6/lib -lX11 -lm

all:    $(OBJS)
	$(CC) -o go  $(OBJS) $(LDFLAGS)  

clean: 
	rm -f *.o *~

.c.o:	$<
	$(CC) $(CFLAGS) -c $<

