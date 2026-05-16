#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>

#include <ncurses.h>

#define BLOCKS_NUM       8 //8 block forms: GENERIC, O, I, S, Z, L, J, T
#define BLOCK_WIDTH      3
#define BLOCK_HEIGHT     4
#define BLOCK_SPEED      2
#define FRAME_PERIOD_MS  16
#define GAME_WIDTH       20
#define GAME_HEIGHT      20
#define BLOCK_MEM_SIZE ((BLOCK_WIDTH+1) * BLOCK_HEIGHT) * 4 //e.g. 4x3 +1 right space, 4 frames

typedef enum blockType{
	GENERIC,
	O,
	I,
	S,
	Z,
	L,
	J,
	T

}blockType;

typedef struct block{
	blockType type;
	int rotation;
	int color;
	float PosX;
	float PosY;

}block;
typedef struct cell{
	int color;
	int x;
	int y;

}cell;

//TODO: L & J and S & Z can be merged
//TODO: \n can be replaced by empty space
char blockGraphics[BLOCKS_NUM][BLOCK_MEM_SIZE] = {
	{ /* GENERIC */
		"#XX XXX XXX XXX "
		"X#X XXX XXX XXX "
		"XX# XXX XXX XXX "
		"XXX XXX XXX XXX "
	}, 

	{ /* O */
		"##X ##X ##X ##X "
		"##X ##X ##X ##X "
		"XXX XXX XXX XXX "
		"XXX XXX XXX XXX "
	},

	{ /* I */
		"#XX ### #XX ### "
		"#XX XXX #XX XXX "
		"#XX XXX #XX XXX "
		"#XX XXX #XX XXX "
	},

	{ /* S */
		"X## X#X X## X#X "
		"##X X## ##X X## "
		"XXX XX# XXX XX# "
		"XXX XXX XXX XXX "
	},

	{ /* Z */
		"##X XXX XXX XXX "
		"X## XXX XXX XXX "
		"XXX XXX XXX XXX "
		"XXX XXX XXX XXX "
	},

	{ /* L */
		"#XX XXX XXX XXX "
		"#XX XXX XXX XXX "
		"##X XXX XXX XXX "
		"XXX XXX XXX XXX "
	},

	{ /* J */
		"X#X XXX XXX XXX "
		"X#X XXX XXX XXX "
		"##X XXX XXX XXX "
		"XXX XXX XXX XXX "
	},

	{ /* T */
		"### XX# X#X #XX "
		"X#X X## ### ##X "
		"XXX XX# XXX #XX "
		"XXX XXX XXX XXX "
	}
};

void CloseWindows();
int GetRealBlockSize();
bool IsBlockColliding();

WINDOW *gameWindow;
block newBlock = {0};
cell occupiedCells[GAME_WIDTH*GAME_HEIGHT] = {0};

int main()
{
	int terminalWidth  = 0;
	int terminalHeight = 0;
	float yCursor = 1;
	float xCursor = GAME_WIDTH/2-1;

	setlocale(LC_ALL,"");
	initscr();
	noecho();
	raw();
	curs_set(0);
	move(0,0);

	getmaxyx(stdscr, terminalHeight, terminalWidth);
	
	if(terminalWidth < GAME_WIDTH +2 || terminalHeight < GAME_HEIGHT + 5){
		printw("Terminal too small...\nPress any key to continue\n");
		getch();
		CloseWindows();
		return 1;
	}
	
	gameWindow = newwin(GAME_HEIGHT,GAME_WIDTH,terminalHeight-GAME_HEIGHT,(terminalWidth-GAME_WIDTH)*.5);
	for(int i = 0; i < GAME_WIDTH; ++i){
	 	occupiedCells[i].y = GAME_HEIGHT -1; 
		occupiedCells[i].x = i;
		occupiedCells[i].color = 0x000000;	
	}

	newBlock.type = T;
	newBlock.rotation = 0;
	box(gameWindow,0,0);
	wrefresh(gameWindow);
	timeout(FRAME_PERIOD_MS);
	double delta  = FRAME_PERIOD_MS/1e3;
	struct timespec start, stop;
	for(;;){
		clock_gettime(CLOCK_MONOTONIC,&start);
		int input = getch();   	

		if(input == 'e' || input == 'E') break;
		if(input == 'r' || input == 'R'){
			++newBlock.rotation;
			while(xCursor + GetRealBlockSize() > GAME_WIDTH-1) --xCursor;
			if(newBlock.rotation >= 4) newBlock.rotation = 0;
		}else if(input == 'l' || input == 'L'){
			++xCursor;
			if(xCursor + GetRealBlockSize() > GAME_WIDTH-1) --xCursor;
		}else if(input == 'h' || input == 'H'){
			--xCursor;
			if(xCursor < 1) xCursor = 1;
		}

		werase(gameWindow);
		wmove(gameWindow,yCursor,xCursor);
		box(gameWindow,0,0);
		
		float currentY = yCursor;
		for(int row = 0; row < 4; row++){
			for(int column = 0 + newBlock.rotation * 4; column < 4 + newBlock.rotation*4; ++column){
				char point = blockGraphics[newBlock.type][row*(BLOCK_MEM_SIZE/4)+ column];
				if(point == '#') wprintw(gameWindow,"%s","▓");
				if(point == ' ') wmove(gameWindow,++currentY,xCursor);
				if(point == 'X') wmove(gameWindow,getcury(gameWindow),getcurx(gameWindow) + 1);
			}
		}
		yCursor += BLOCK_SPEED* delta;

		wrefresh(gameWindow);
		clock_gettime(CLOCK_MONOTONIC,&stop);
		delta = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec)/1e9;
		if(delta*1e3 < FRAME_PERIOD_MS) sleep(FRAME_PERIOD_MS/1e3 - delta);
	}
	
	CloseWindows();
	return 0;

}

int GetRealBlockSize(){
	int currentBlockWidth = 1;
	for(int row = 0; row < 4; ++row){
		for (int column = 0; column < BLOCK_WIDTH; ++column){
			char point = blockGraphics[newBlock.type][(row * (BLOCK_MEM_SIZE/4) + newBlock.rotation * 4) + column];
			if(point == '#' && currentBlockWidth < column+1) ++currentBlockWidth;
		}
	}


	return currentBlockWidth;
}

bool IsBlockColliding(){
	/*
	{ 
		"#XX XXX XXX XXX "
		"X#X XXX XXX XXX "
		"XX# XXX XXX XXX "
		"XXX XXX XXX XXX "
	} */

	for(int i = 0; i < GAME_WIDTH*GAME_HEIGHT; ++i){
		c = occupiedCells[i];
		for(int row = 0; row < BLOCK_HEIGHT; ++row){
			for(int column = 0; column < BLOCK_WIDTH; ++column){
			
				blockGraphics[newBlock.type];
			}
		}	

	}
	return false;
}



void CloseWindows(){
	endwin();
	delwin(gameWindow);
}
