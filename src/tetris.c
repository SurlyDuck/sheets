#define _DEFAULT_SOURCE
#define BLOCKS_NUM      8 //8 block forms: GENERIC, O, I, S, Z, L, J, T
#define BLOCK_WIDTH     3
#define BLOCK_HEIGHT    4
#define FRAME_PERIOD_MS 16
#define BLOCK_MEM_SIZE ((BLOCK_WIDTH+1) * BLOCK_HEIGHT) * 4 //e.g. 4x3 +1 right space, 4 frames

#include <stdlib.h>
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
		"### ### ### ###\n"
		"### ### ### ###\n"
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
		"### XXX XXX XXX\n"
		"### XXX XXX XXX\n"
		"### XXX XXX XXX\n"
		"### XXX XXX XXX\n"
	},

	{ /* T */
		"### XXX XXX XXX\n"
		"X#X XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
	}
};


int main(){
	setlocale(LC_ALL,"");
	initscr();
	noecho();
	raw();
	curs_set(0);
	move(0,0);
	
	block newBlock = {0};
	newBlock.type = J;
	
	timeout(FRAME_PERIOD_MS);
	float yCursor = 0;
	float xCursor = 0;
	double delta  = FRAME_PERIOD_MS/1e3;
	for(;;){
		clock_gettime(CLOCK_MONOTONIC,&start);
		int input = getch();   	
		if(input == 'e') break;
		erase();
		move(yCursor,xCursor);
		for(int row = 0; row < 4; row++){
			for(int column = 0; column < 4; ++column){
				char point = blockGraphics[newBlock.type][row*16 + column];
				if(point == '#') printw("%s","▓");
				if(point == ' ') move(yCursor + 1 ,xCursor);
				if(point == 'X') move(getcury(stdscr),xCursor+1);
			}
		}
		yCursor += .5 * delta;
		xCursor  = 0;
		clock_gettime(CLOCK_MONOTONIC,&stop);
		delta = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec)/1e9;
		if(delta*1e3 < FRAME_PERIOD_MS) sleep(FRAME_PERIOD_MS/1e3 - delta);
	}

	endwin();


	return 0;

}
