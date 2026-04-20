#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define YOFFSET         5
#define PERIOD_MS      16
#define PLAYER_SPEED     .3
#define MAX_SIZE       10

WINDOW *gameWindow;
WINDOW *CreateWindow(int width, int height, int ix, int iy);

typedef struct v2{
	float x;
	float y;
}v2;


const char *gameMsg = "Welcome to Snake Sheet! Press 'e' to quit";

int main(int argc, char **argv){
	int width, height, startx,starty = 0;

	initscr();
	noecho();
	raw();
	curs_set(0);
	getmaxyx(stdscr,height,width);
	attron(A_BOLD);
	mvprintw(0, (width-strlen(gameMsg))/2,gameMsg);
	attroff(A_BOLD);
	refresh();
	startx  = (width - width/2)/2;
	height /= 2;
	width  /= 2;
	gameWindow = CreateWindow(width,height,startx,starty+YOFFSET);
	
	v2 playerPos = {width/2,(starty+height/2)};
	v2 playerDir = {1,0};
	timeout(PERIOD_MS);
	struct timespec start,stop;
	for(;;){
		clock_gettime(CLOCK_MONOTONIC,&start);
		int input = getch();
		playerPos.x += PLAYER_SPEED;
		if(input == 'e') break;
		werase(gameWindow);
		box(gameWindow,0,0);
		mvwaddch(gameWindow,playerPos.y,playerPos.x,'@');
		wrefresh(gameWindow);

		clock_gettime(CLOCK_MONOTONIC,&stop);
		double elapsed   = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec)/1e9;
		double remaining = (double)(PERIOD_MS/1e3) - elapsed;
		if (remaining > 0) sleep(remaining);
		
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
