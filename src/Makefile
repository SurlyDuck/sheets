../bin/main:  main.c
	gcc -std=c99 -Wall -pedantic -o ../bin/main main.c -Wl,-Bstatic -lncursesw -Wl,-Bdynamic && ../bin/main

clean:
	rm ../bin/main && make
