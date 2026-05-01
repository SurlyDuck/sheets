#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#define TOP_CROP_SIZE          6
#define FRAME_PERIOD_MS        16
#define PLAYER_SPEED           .1
#define SPEED_BOOST            .5
#define MAX_PARTS              2048
#define GREEN_RGB              78,521,149
#define PAUSE_WINDOW_WIDTH     24
#define PAUSE_WINDOW_HEIGHT    8
#define NEW_GAME_WINDOW_WIDTH  30
#define NEW_GAME_WINDOW_HEIGHT 10
#define INFO_PANEL_HEIGHT      5 

typedef struct v2{
	float x;
	float y;
}v2;

typedef enum state{
	GAMEPLAY      = 0,
	GAME_OVER     = 1,
	EXITING_GAME  = 2,
	PAUSE         = 3,
	OPTIONS      = 4
}state;

WINDOW *CreateWindow(int gameWidth, int gameHeight, int ix, int iy);
v2 SpawnPowerUp(int xConstraint, int yConstraint);
void InitCurses();
void InitGameWindow();
void DrawInfoWindow();
void DrawTitleScreen();
bool IsGameOver();
state UpdateAndDrawGameplay();
state UpdateAndDrawPauseScreen();
state UpdateAndDrawNewGameScreen();

v2 parts[MAX_PARTS];
v2 partsBuffer[MAX_PARTS];
v2 playerDir                               = {1,0};
v2 powerUpPos                              = {0};
state currentGameState                     = GAMEPLAY;
WINDOW *gameWindow                         = NULL;
WINDOW *infoWindow                         = NULL;
WINDOW *pauseWindow                        = NULL;
WINDOW *newGameWindow                      = NULL;
int currentLenght                          = 3;
int gameWidth, gameHeight, startx,starty   = 0;
int newGameWindowSelection                 = 0;
bool canRotate                             = true;
bool enabledWalls, enabledIncrementalSpeed = false;  
const char *gameMsg                        = "Welcome to Snake Sheet! Press 'e' to quit";
struct timespec start,stop;

char titleScreen[] = 
                                             
" ,---.  ,--.  ,--.  ,---.  ,--. ,--.,------. \0" 
"'   .-' |  ,'.|  | /  O  \\ |  .'   /|  .---'  \0" 
"`.  `-. |  |' '  ||  .-.  ||  .   ' |  `--,    \0" 
".-'    ||  | `   ||  | |  ||  |\\   \\|  `---. \0" 
"`-----' `--'  `--'`--' `--'`--' '--'`------'   \0";
//from patorjk.com ASCII generator
                                             
int main(int argc, char **argv){
	InitCurses();

	startx  = (gameWidth - gameWidth/2)/2;
	starty  = TOP_CROP_SIZE;
	gameHeight /= 2;
	gameWidth  /= 2;
	InitGameWindow();
	DrawTitleScreen();
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
			case PAUSE:
				currentGameState = UpdateAndDrawPauseScreen();
				break;
			case OPTIONS:
				currentGameState = UpdateAndDrawNewGameScreen();
				break;
			default: break;
		}
	}	
CLOSING:
	getch();
	delwin(gameWindow);
	delwin(infoWindow);
	delwin(pauseWindow);
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
	if(input        == 'p' || input == 'P') return PAUSE;
	if(input        == 'r' || input == 'R') return GAME_OVER;
	if(input        == 'e' || input == 'E') return EXITING_GAME;
	if(input        == 'n' || input == 'N') return OPTIONS;
	if(IsGameOver()) return GAME_OVER;
	
	if((input == 'w' || input == 's' || input == 'W' || input == 'S') && canRotate && !(int)(playerDir.y)) {
		canRotate = false;
		playerDir.y = (input == 'w' || input == 'W') * -1 + (input == 's' || input == 'S') * 1;
		playerDir.x = 0;
	}
	if((input == 'a' || input == 'd' || input == 'A' || input == 'D') && canRotate  && !(int)(playerDir.x)) {
		canRotate = false;
		playerDir.x = (input == 'a' || input == 'A') * -1 + (input == 'd' || input == 'D') * 1;
		playerDir.y = 0;
	}

	werase(gameWindow);
	box(gameWindow,0,0);

	parts[0].x += playerDir.x * (PLAYER_SPEED + (input == 'f' || input == 'F') * SPEED_BOOST);
	parts[0].y += playerDir.y * (PLAYER_SPEED + (input == 'f' || input == 'F') * SPEED_BOOST);

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
	assert(gameHeight >= 25);
	start_color(); 
	noecho();
	init_color(COLOR_BLACK,0,0,0);
	init_color(COLOR_GREEN,GREEN_RGB);
	init_pair(1,COLOR_GREEN, COLOR_BLACK);
	raw();
	curs_set(0);

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
	const char *windowTitle = "INFO";

	if(infoWindow == NULL){
		infoWindow = CreateWindow(gameWidth,INFO_PANEL_HEIGHT,startx,starty+gameHeight);
		wattron(infoWindow,COLOR_PAIR(1));
	}werase(infoWindow);

		box(infoWindow,0,0);
		mvwprintw(infoWindow,0,(gameWidth-strlen(windowTitle))/2,windowTitle);
		mvwprintw(infoWindow,1,1,"%s: %*s","WASD", 5, "Move");
		mvwprintw(infoWindow,2,1,"%s: %*s","E", 8, "Exit");
		mvwprintw(infoWindow,3,1,"%s: %*s","N", 12, "New Game");
		mvwprintw(infoWindow,1,gameWidth-12,"F:  Boost");
		mvwprintw(infoWindow,2,gameWidth-12,"P:  Pause");
		mvwprintw(infoWindow,3,gameWidth-12,"R:  Restart");
		wrefresh(infoWindow);
	
}

state UpdateAndDrawPauseScreen(){
	
	int input = getch();
	const char *windowTitle  = "PAUSED";

	if(input == 'E' || input == 'e') return EXITING_GAME;
	if(input == 'P' || input == 'p'){
		werase(pauseWindow); 

	   wborder(pauseWindow, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
		wrefresh(pauseWindow);
		return GAMEPLAY;
	}
	if(pauseWindow == NULL){
		pauseWindow   = CreateWindow(PAUSE_WINDOW_WIDTH, PAUSE_WINDOW_HEIGHT,startx+(gameWidth-PAUSE_WINDOW_WIDTH)*.5,(starty) + (gameHeight-PAUSE_WINDOW_HEIGHT)*.5); 
		wattron(pauseWindow,COLOR_PAIR(1));
	}else werase(pauseWindow);

	box(pauseWindow,0,0);
	mvwprintw(pauseWindow,0,(PAUSE_WINDOW_WIDTH-strlen(windowTitle))*.5,windowTitle);
	mvwprintw(pauseWindow,1,1,"Snake by Lucas!");
	mvwprintw(pauseWindow,2,1,"Check my other games");
	mvwprintw(pauseWindow,3,1,"Have fun!");
	mvwprintw(pauseWindow,PAUSE_WINDOW_HEIGHT - 2,PAUSE_WINDOW_WIDTH-5,"2026");
	wrefresh(pauseWindow);

	return PAUSE;

}

void DrawTitleScreen(){
	v2 cursorPos = {0};
	int titleWidth = 0;
	int cursorInitialPosX = 0;

	for(int i = 0; titleScreen[i] != '\0'; ++i) ++titleWidth;
	cursorInitialPosX = (startx) + (gameWidth - titleWidth)*.5;
	cursorPos.y = starty - 5; 
	cursorPos.x = cursorInitialPosX ;
	
	attron(COLOR_PAIR(1));
	for(int i = 0; i < sizeof(titleScreen)/sizeof(char); ++i){
		if (titleScreen[i] == '\0') {
			++cursorPos.y;
			cursorPos.x = cursorInitialPosX;
			refresh();
			continue;
		}
		mvaddch(cursorPos.y,cursorPos.x,titleScreen[i]);
		++cursorPos.x;
	}
	attroff(COLOR_PAIR(1));
	refresh();

}

state UpdateAndDrawNewGameScreen(){
   wtimeout(newGameWindow,FRAME_PERIOD_MS);
	const char *windowTitle  = "New Game";
	const int  choices       = 2;
	int input                = wgetch(newGameWindow);
	//TODO: this is very bad, needs refactoring.
	//i'm very sorry
	if(input == KEY_UP || input == 'W' || input == 'w')    ++newGameWindowSelection;
	if(input == KEY_DOWN || input == 'S' || input == 's')  --newGameWindowSelection;
	if(input == 'c' || input == 'C') {
		if(newGameWindowSelection == 0) enabledWalls = !enabledWalls;
		if(newGameWindowSelection == 1) enabledIncrementalSpeed = !enabledIncrementalSpeed;
	}
	if(newGameWindowSelection > choices-1)    newGameWindowSelection = 0;
	if(newGameWindowSelection < 0)            newGameWindowSelection = choices-1;
	
   if(input == 'E' || input == 'e'){
   	werase(newGameWindow); 
      wborder(newGameWindow, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
   	wrefresh(newGameWindow);
   	return GAMEPLAY;
   }
   if(newGameWindow == NULL){
   	newGameWindow   = CreateWindow(NEW_GAME_WINDOW_WIDTH, NEW_GAME_WINDOW_HEIGHT,startx+(gameWidth-NEW_GAME_WINDOW_WIDTH)*.5,(starty) + (gameHeight-NEW_GAME_WINDOW_HEIGHT)*.5); 
   	wattron(newGameWindow,COLOR_PAIR(1));
		keypad(newGameWindow,TRUE);
		
   }else werase(newGameWindow);

   box(newGameWindow,0,0);
   mvwprintw(newGameWindow,0,(NEW_GAME_WINDOW_WIDTH-strlen(windowTitle))*.5,windowTitle);
   mvwprintw(newGameWindow,1,1,"(%c) Random Walls%s",((enabledWalls) ? '*' : ' '),((newGameWindowSelection == 0) ? " <--"  : " "));
   mvwprintw(newGameWindow,2,1,"(%c) Increment Speed%s",((enabledIncrementalSpeed) ? '*' : ' '), ((newGameWindowSelection == 1) ? " <--" : " "));
   mvwprintw(newGameWindow,NEW_GAME_WINDOW_HEIGHT-2,1,"%s %*s", "C:", 10,"Change");
   mvwprintw(newGameWindow,NEW_GAME_WINDOW_HEIGHT-3,1,"%s %*s", "E:", 8,"Exit");
   mvwprintw(newGameWindow,NEW_GAME_WINDOW_HEIGHT-4,1,"%s %*s", "SPACE:", 7,"Confirm");
	wrefresh(newGameWindow);

	return OPTIONS;
}














