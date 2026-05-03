CFLAGS := -g -O2 -std=c99 -Wall -pedantic 
LFLAGS := -lncursesw 
OBJ    := ../bin/snake ../bin/tetris
CC     := gcc

all: ../bin/snake ../bin/tetris

../bin/snake:  snake.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)
	#../bin/snake

../bin/tetris: tetris.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)
	../bin/tetris

clean:
	rm $(OBJ) && make
