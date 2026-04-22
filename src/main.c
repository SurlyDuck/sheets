#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#define Y_PADDING       5
#define PERIOD_MS      16
#define PLAYER_SPEED     .1
#define MAX_PARTS      256

typedef struct v2{
	float x;
	float y;
}v2;

WINDOW *CreateWindow(int width, int height, int ix, int iy);

v2 parts[MAX_PARTS];
v2 partsBuffer[MAX_PARTS];
int currentLenght = 20;
const char *gameMsg = "Welcome to Snake Sheet! Press 'e' to quit";

int main(int argc, char **argv){
	int width, height, startx,starty = 0;
	bool canRotate = true;
	WINDOW *gameWindow;
	initscr();
	assert(has_colors());
	start_color(); 
	noecho();
	init_color(COLOR_BLACK,0,0,0);
	init_color(COLOR_GREEN,78,521,149);
	init_pair(1,COLOR_GREEN, COLOR_BLACK);
	raw();
	curs_set(0);
	getmaxyx(stdscr,height,width);
	attron(A_BOLD | COLOR_PAIR(1));
	mvprintw(0, (width-strlen(gameMsg))/2,gameMsg);
	attroff(A_BOLD);

	refresh();

	startx  = (width - width/2)/2;
	height /= 1.5;
	width  /= 2;
	gameWindow = CreateWindow(width,height,startx,starty+Y_PADDING);
	wattron(gameWindow,COLOR_PAIR(1));
	v2 playerPos = {width/2,(starty+height/1.5)};
	for(int i =0; i < MAX_PARTS; ++i){
		parts[i]    = playerPos;
		parts[i].x -= i;
	}
   memcpy( partsBuffer, parts, sizeof(parts));
	
	v2 playerDir = {0,0};
	float rotation = 0;
	timeout(PERIOD_MS);
	struct timespec start,stop;

	for(;;){
		clock_gettime(CLOCK_MONOTONIC,&start);
		int input       = getch();

		if(input        == 'e') break;
		if(input        == 'l' && canRotate) {
			rotation +=(float)(M_PI/2); 
			canRotate = false; 
		}
		if(input        == 'h' && canRotate) {
			rotation -=(float)(M_PI/2);
			canRotate = false;
		}

		playerDir        = (v2) {cosf(rotation), sinf(rotation)};
		werase(gameWindow);
		box(gameWindow,0,0);
		parts[0].x += playerDir.x * PLAYER_SPEED;
		parts[0].y += playerDir.y * PLAYER_SPEED;

		if((int)(partsBuffer[0].x) != (int)(parts[0].x) || (int)(partsBuffer[0].y) != (int)(parts[0].y)){
			for (int i = 1; i < currentLenght; ++i){
				parts[i].x = partsBuffer[i-1].x;
				parts[i].y = partsBuffer[i-1].y;
			}
			memcpy( partsBuffer, parts, sizeof(parts));
			canRotate = true;
		}
		for(int i =0; i < currentLenght; ++i) {
			
			mvwaddch(gameWindow,parts[i].y,parts[i].x,'@');
		}
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
