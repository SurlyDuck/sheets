#define _DEFAULT_SOURCE
#define BLOCKS_NUM       8 //8 block forms: GENERIC, O, I, S, Z, L, J, T
#define BLOCK_WIDTH      3
#define BLOCK_HEIGHT     4
#define BLOCK_SPEED      2
#define FRAME_PERIOD_MS  16
#define GAME_WIDTH       20
#define GAME_HEIGHT      20
#define BLOCK_MEM_SIZE ((BLOCK_WIDTH+1) * BLOCK_HEIGHT) * 4 //e.g. 4x3 +1 right space, 4 frames

#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>

struct timespec start, stop;

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
//TODO: L & J and S & Z can be merged
//TODO: \n can be replaced by empty space
char blockGraphics[BLOCKS_NUM][BLOCK_MEM_SIZE] = {
	{ /* GENERIC */
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
	}, 

	{ /* O */
		"##X ### ### ###\n"
		"##X ### ### ###\n"
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
	},

	{ /* I */
		"#XX ### #XX ###\n"
		"#XX XXX #XX XXX\n"
		"#XX XXX #XX XXX\n"
		"#XX XXX #XX XXX\n"
	},

	{ /* S */
		"X## XXX XXX XXX\n"
		"##X XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
	},

	{ /* Z */
		"##X XXX XXX XXX\n"
		"X## XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
	},

	{ /* L */
		"#XX XXX XXX XXX\n"
		"#XX XXX XXX XXX\n"
		"##X XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
	},

	{ /* J */
		"X#X XXX XXX XXX\n"
		"X#X XXX XXX XXX\n"
		"##X XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
	},

	{ /* T */
		"### XXX XXX XXX\n"
		"X#X XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
	}
};

void CloseWindows();


WINDOW *gameWindow;

int main()
{
	int terminalWidth  = 0;
	int terminalHeight = 0;

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
	block newBlock = {0};
	newBlock.type = O;
	box(gameWindow,0,0);
	wrefresh(gameWindow);
	timeout(FRAME_PERIOD_MS);
	float yCursor = 1;
	float xCursor = 1;
	double delta  = FRAME_PERIOD_MS/1e3;
	for(;;){
		clock_gettime(CLOCK_MONOTONIC,&start);
		int input = getch();   	
		if(input == 'e') break;
		werase(gameWindow);
		wmove(gameWindow,yCursor,xCursor);
		box(gameWindow,0,0);

		for(int row = 0; row < 4; row++){
			for(int column = 0; column < 4; ++column){
				char point = blockGraphics[newBlock.type][row*16 + column];
				if(point == '#') wprintw(gameWindow,"%s","▓");
				if(point == ' ') wmove(gameWindow,yCursor + 1 ,xCursor);
				if(point == 'X') wmove(gameWindow,getcury(gameWindow),getcurx(gameWindow) + 1);
			}
		}
		yCursor += BLOCK_SPEED* delta;
		xCursor  = 1;

		wrefresh(gameWindow);
		clock_gettime(CLOCK_MONOTONIC,&stop);
		delta = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec)/1e9;
		if(delta*1e3 < FRAME_PERIOD_MS) sleep(FRAME_PERIOD_MS/1e3 - delta);
	}
	
	CloseWindows();
	return 0;

}

void CloseWindows(){
	endwin();
	delwin(gameWindow);
}
