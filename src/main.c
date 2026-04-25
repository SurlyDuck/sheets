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

#define TOP_CROP_SIZE         5
#define FRAME_PERIOD_MS      16
#define PLAYER_SPEED          .1
#define SPEED_BOOST           .5
#define MAX_PARTS           256
#define GREEN_RGB           78,521,149

typedef struct v2{
	float x;
	float y;
}v2;

typedef enum state{
	GAMEPLAY   = 0,
	GAME_OVER  = 1
}state;

WINDOW *CreateWindow(int width, int height, int ix, int iy);
v2 SpawnPowerUp(int xConstraint, int yConstraint);
void InitCurses();
bool IsGameOver();

v2 parts[MAX_PARTS];
v2 partsBuffer[MAX_PARTS];
int currentLenght = 3;
int width, height, startx,starty = 0;
const char *gameMsg = "Welcome to Snake Sheet! Press 'e' to quit";

int main(int argc, char **argv){
	bool canRotate = true;
	WINDOW *gameWindow;
	v2 playerDir   = {1,0};
	v2 powerUpPos  = {0};
	struct timespec start,stop;

	InitCurses();

	startx  = (width - width/2)/2;
	height /= 1.5;
	width  /= 2;
	gameWindow = CreateWindow(width,height,startx,starty+TOP_CROP_SIZE);
	wattron(gameWindow,COLOR_PAIR(1));
	v2 playerPos = {width/2,(starty+height/1.5)};
	for(int i =0; i < MAX_PARTS; ++i){
		parts[i]    = playerPos;
		parts[i].x -= i;
	}
	powerUpPos = SpawnPowerUp(width-2,height-2);
   memcpy( partsBuffer, parts, sizeof(parts));
	
	timeout(FRAME_PERIOD_MS);

	for(;;){
		clock_gettime(CLOCK_MONOTONIC,&start);
		int input       = getch();
		if(input        == 'e') break;
		if(IsGameOver()) break;
		if((input == 'w' || input == 's') && canRotate && !(int)(playerDir.y)) {
			canRotate = false;
			playerDir.y = (input == 'w') * -1 + (input == 's') * 1;
			playerDir.x = 0;
		}
		if((input == 'a' || input == 'd') && canRotate  && !(int)(playerDir.x)) {
			canRotate = false;
			playerDir.x = (input == 'a') * -1 + (input == 'd') * 1;
			playerDir.y = 0;
		}

		werase(gameWindow);
		box(gameWindow,0,0);

		parts[0].x += playerDir.x * (PLAYER_SPEED + (input == 'f') * SPEED_BOOST);
		parts[0].y += playerDir.y * (PLAYER_SPEED + (input == 'f') * SPEED_BOOST);
		
		if((int)(parts[0].x) == (int)(powerUpPos.x) && (int)(parts[0].y) == (int)(powerUpPos.y) ){
			powerUpPos = SpawnPowerUp(width-2, height-2);
			++currentLenght;
		}

		if((int)(parts[0].x) >= width-1) parts[0].x = 1;
		if((int)(parts[0].x) < 1) parts[0].x = width-2;
		if((int)(parts[0].y) >= height-1) parts[0].y = 1;
		if((int)(parts[0].y) < 1) parts[0].y = height-2;

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
		mvwaddch(gameWindow,powerUpPos.y,powerUpPos.x,ACS_DIAMOND);
		wrefresh(gameWindow);

		clock_gettime(CLOCK_MONOTONIC,&stop);
		double elapsed   = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec)/1e9;
		double remaining = (double)(FRAME_PERIOD_MS/1e3) - elapsed;
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

v2 SpawnPowerUp(int xConstraint, int yConstraint){
	srand(arc4random());	
	v2 powerUpPos = {0};
	powerUpPos.x = (int)(1+rand() / (RAND_MAX / (xConstraint)));
	powerUpPos.y = (int)(1+rand() / (RAND_MAX / (yConstraint)));

	for(int i = 0; i < currentLenght; i++){
		if((int)(parts[i].x) == (int)(powerUpPos.x) && (int)(parts[i].y) == (int)(powerUpPos.y)){
			i = 0;
			powerUpPos.x = (int)(1+rand() / (RAND_MAX / (xConstraint)));
			powerUpPos.y = (int)(1+rand() / (RAND_MAX / (yConstraint)));
		
		}
	}
	return powerUpPos;
	
}

bool IsGameOver(){
	for(int i = 1; i < currentLenght; ++i){
		if((int)(parts[0].x) == (int)(parts[i].x) && (int)(parts[0].y) == (int)(parts[i].y)) return true; 
	}

	return false;		

}

void UpdateAndDrawGameplay(){

}

void UpdateAndDrawGameOver(){

}

void InitCurses(){
	initscr();
	assert(has_colors());
	start_color(); 
	noecho();
	init_color(COLOR_BLACK,0,0,0);
	init_color(COLOR_GREEN,GREEN_RGB);
	init_pair(1,COLOR_GREEN, COLOR_BLACK);
	raw();
	curs_set(0);
	getmaxyx(stdscr,height,width);
	attron(A_BOLD | COLOR_PAIR(1));
	mvprintw(0, (width-strlen(gameMsg))/2,gameMsg);
	attroff(A_BOLD);

	refresh();
}







