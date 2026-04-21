#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

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
int currentLenght = 10;
const char *gameMsg = "Welcome to Snake Sheet! Press 'e' to quit";

int main(int argc, char **argv){
	int width, height, startx,starty = 0;
	WINDOW *gameWindow;

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
	height /= 1.5;
	width  /= 2;
	gameWindow = CreateWindow(width,height,startx,starty+Y_PADDING);

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
		bool shouldMove = false;
		if(input        == 'e') break;
		if(input        == 'l') rotation +=(float)(M_PI/2);   
		if(input        == 'h') rotation -=(float)(M_PI/2);
		playerDir        = (v2) {cosf(rotation), sinf(rotation)};
		//playerPos.x += playerDir.x * PLAYER_SPEED;
		//playerPos.y += playerDir.y * PLAYER_SPEED;
		werase(gameWindow);
		box(gameWindow,0,0);
		//mvwaddch(gameWindow,playerPos.y,playerPos.x,'@');
		parts[0].x += playerDir.x * PLAYER_SPEED;
		parts[0].y += playerDir.y * PLAYER_SPEED;
	 	//mvwaddch(gameWindow,parts[0].y,parts[0].x,'@');

		if((int)(partsBuffer[0].x) != (int)(parts[0].x) || (int)(partsBuffer[0].y) != (int)(parts[0].y)){
			for (int i = 1; i < currentLenght; ++i){
				parts[i].x = partsBuffer[i-1].x;
				parts[i].y = partsBuffer[i-1].y;
			}
			memcpy( partsBuffer, parts, sizeof(parts));
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
