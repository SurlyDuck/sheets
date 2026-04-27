#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#define TOP_CROP_SIZE          5
#define FRAME_PERIOD_MS       16
#define PLAYER_SPEED            .1
#define SPEED_BOOST             .5
#define MAX_PARTS           2048
#define GREEN_RGB           78,521,149

typedef struct v2{
	float x;
	float y;
}v2;

typedef enum state{
	GAMEPLAY      = 0,
	GAME_OVER     = 1,
	EXITING_GAME  = 2
}state;

WINDOW *CreateWindow(int gameWidth, int gameHeight, int ix, int iy);
v2 SpawnPowerUp(int xConstraint, int yConstraint);
void InitCurses();
void InitGameWindow();
void DrawInfoWindow();
bool IsGameOver();
state UpdateAndDrawGameplay();

v2 parts[MAX_PARTS];
v2 partsBuffer[MAX_PARTS];
v2 playerDir   = {1,0};
v2 powerUpPos  = {0};
state currentGameState = GAMEPLAY;
WINDOW *gameWindow = NULL;
WINDOW *infoWindow = NULL;
int currentLenght = 3;
int gameWidth, gameHeight, startx,starty = 0;
bool canRotate = true;
const char *gameMsg = "Welcome to Snake Sheet! Press 'e' to quit";
struct timespec start,stop;

int main(int argc, char **argv){
	InitCurses();

	startx  = (gameWidth - gameWidth/2)/2;
	starty  = TOP_CROP_SIZE;
	gameHeight /= 1.5;
	gameWidth  /= 2;
	InitGameWindow();
	DrawInfoWindow();
	timeout(FRAME_PERIOD_MS);
	for(;;){
		switch(currentGameState){
			case GAMEPLAY:
				currentGameState = UpdateAndDrawGameplay();
				break;
			case GAME_OVER:
				InitGameWindow();
				currentGameState = GAMEPLAY;
				break;
			case EXITING_GAME:
				goto CLOSING;
				break;
			default: break;
		}
	}	
CLOSING:
	getch();
	delwin(gameWindow);
	delwin(infoWindow);
	endwin();

}

WINDOW *CreateWindow(int gameWidth, int gameHeight, int ix, int iy){
	WINDOW *localWin = newwin(gameHeight,gameWidth,iy,ix);
	
	box(localWin,0,0);
	wrefresh(localWin);
	
	return localWin;

}

v2 SpawnPowerUp(int xConstraint, int yConstraint){
	srand(arc4random());	
	v2 _powerUpPos = {0};
	_powerUpPos.x = (int)(1+rand() / (RAND_MAX / (xConstraint)));
	_powerUpPos.y = (int)(1+rand() / (RAND_MAX / (yConstraint)));

	for(int i = 0; i < currentLenght; i++){
		if((int)(parts[i].x) == (int)(_powerUpPos.x) && (int)(parts[i].y) == (int)(_powerUpPos.y)){
			i = 0;
			_powerUpPos.x = (int)(1+rand() / (RAND_MAX / (xConstraint)));
			_powerUpPos.y = (int)(1+rand() / (RAND_MAX / (yConstraint)));
		
		}
	}
	return _powerUpPos;
	
}

bool IsGameOver(){
	for(int i = 1; i < currentLenght; ++i){
		if((int)(parts[0].x) == (int)(parts[i].x) && (int)(parts[0].y) == (int)(parts[i].y)) return true; 
	}

	return false;		

}
state UpdateAndDrawGameplay(){
	clock_gettime(CLOCK_MONOTONIC,&start);
	int input       = getch();
	if(input        == 'e') return EXITING_GAME;
	if(IsGameOver()) return GAME_OVER;
	
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
		powerUpPos = SpawnPowerUp(gameWidth-2, gameHeight-2);
		++currentLenght;
	}

	if((int)(parts[0].x) >= gameWidth-1) parts[0].x = 1;
	if((int)(parts[0].x) < 1) parts[0].x = gameWidth-2;
	if((int)(parts[0].y) >= gameHeight-1) parts[0].y = 1;
	if((int)(parts[0].y) < 1) parts[0].y = gameHeight-2;

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
	
	return GAMEPLAY;
}

void UpdateAndDrawGameOver(){
	//TODO
}

void InitCurses(){
	initscr();
	getmaxyx(stdscr,gameHeight,gameWidth);
	assert(has_colors());
	assert(gameHeight >= 28);
	start_color(); 
	noecho();
	init_color(COLOR_BLACK,0,0,0);
	init_color(COLOR_GREEN,GREEN_RGB);
	init_pair(1,COLOR_GREEN, COLOR_BLACK);
	raw();
	curs_set(0);
	//attron(A_BOLD | COLOR_PAIR(1));
	//mvprintw(0, (gameWidth-strlen(gameMsg))/2,gameMsg);
	//attroff(A_BOLD);

	refresh();
}

void InitGameWindow(){
	v2 playerPos = {gameWidth/2,(gameHeight/2)};
	currentLenght = 3;
	for(int i =0; i < MAX_PARTS; ++i){
		parts[i]    = playerPos;
		parts[i].x -= i;
	}

	memcpy( partsBuffer, parts, sizeof(parts));

	if(gameWindow == NULL){
		gameWindow = CreateWindow(gameWidth,gameHeight,startx,starty);
		wattron(gameWindow,COLOR_PAIR(1));
	}else(werase(gameWindow));
	
	wrefresh(gameWindow);
	powerUpPos = SpawnPowerUp(gameWidth-2,gameHeight-2);
	playerDir  = (v2) {1,0};
}

void DrawInfoWindow(){
	const char *msg = "INFO";

	if(infoWindow == NULL){
		infoWindow = CreateWindow(gameWidth,5,startx,starty+gameHeight);
		wattron(infoWindow,COLOR_PAIR(1));
	}werase(infoWindow);

		box(infoWindow,0,0);
		mvwprintw(infoWindow,0,(gameWidth-strlen(msg))/2,msg);
		mvwprintw(infoWindow,1,1,"Mov.: WASD");
		mvwprintw(infoWindow,2,1,"Exit: E");
		wrefresh(infoWindow);
	
}











