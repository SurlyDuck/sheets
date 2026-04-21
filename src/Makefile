../bin/main:  main.c
	gcc -g -std=c99 -Wall -pedantic -o ../bin/main main.c -Wl,-Bstatic -lncursesw -Wl,-Bdynamic -lm
	../bin/main

clean:
	rm ../bin/main && make
