#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define YOFFSET   5
#define PERIOD_MS 16

WINDOW *gameWindow;
WINDOW *CreateWindow(int width, int height, int ix, int iy);

typedef struct v2{
	int x;
	int y;
}v2;


const char *gameMsg = "@@@@@ SNAKE! @@@@@";

int main(int argc, char **argv){
	int width, height, startx,starty = 0;

	initscr();
	noecho();
	raw();
	curs_set(0);
	getmaxyx(stdscr,height,width);
	attron(A_BOLD | A_BLINK);
	mvprintw(0, (width-strlen(gameMsg))/2,gameMsg);
	attroff(A_BOLD | A_BLINK);
	refresh();
	height /= 2;
	gameWindow = CreateWindow(width,height,startx,starty+YOFFSET);
	
	mvaddch(height/2+YOFFSET,width/2,'@');
	timeout(PERIOD_MS);
	struct timespec start,stop;
	clock_gettime(CLOCK_MONOTONIC,&start);
	for(;;){
		int input = getch();
		
		if(input == 'e') break;
		
		clock_gettime(CLOCK_MONOTONIC,&stop);
		double elapsed = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec)/1e9;
 		if(elapsed > 5) break; //breaks after 5 minutes
	}	

	getch();
	delwin(gameWindow);
	endwin();

}

WINDOW *CreateWindow(int width, int height, int ix, int iy){
	WINDOW *localWin = newwin(height,width,iy,ix);
	
	box(localWin,0,0);
	wrefresh(localWin);
	
	return localWin;

}
