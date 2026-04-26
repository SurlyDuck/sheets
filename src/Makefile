CFLAGS := -g -O2 -std=c99 -Wall -pedantic 
LFLAGS := -Wl,-Bstatic -lncursesw -Wl,-Bdynamic 

all: ../bin/main

../bin/main:  main.c
	gcc $(CFLAGS) -o $@ $^ $(LFLAGS)
	../bin/main

clean:
	rm ../bin/main && make
