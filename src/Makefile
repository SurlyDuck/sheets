CFLAGS := -g -O2 -std=c99 -Wall -pedantic 
LFLAGS := -lncursesw 
OBJ    := ../bin/snake

all: ../bin/snake

../bin/snake:  snake.c
	gcc $(CFLAGS) -o $@ $^ $(LFLAGS)
	../bin/snake

clean:
	rm $(OBJ) && make
