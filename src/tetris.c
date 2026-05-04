#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>

#define BLOCKS_NUM   8
#define BLOCK_WIDTH  3
#define BLOCK_HEIGHT 4
#define BLOCK_MEM_SIZE ((BLOCK_WIDTH+1) * BLOCK_HEIGHT) * 4 //e.g. 4x3 +1 right space, 4 rotations

typedef enum blockType{
	FULL,
	O,
	I

}blockType;

typedef struct block{
	blockType type;
	int rotation;
	int color;
	float PosX;
	float PosY;

}block;

char blockGraphics[BLOCKS_NUM][BLOCK_MEM_SIZE] = {
	{ /* GENERIC */
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
	}, 

	{ /* O */
		"XXX XXX XXX XXX\n"
		"XXX XXX XXX XXX\n"
		"##X ##X ##X ##X\n"
		"##X ##X ##X ##X\n"
	},

	{ /* I */
		"#XX XXX #XX XXX\n"
		"#XX ### #XX ###\n"
		"#XX XXX #XX XXX\n"
		"#XX XXX #XX XXX\n"
	}
};


char blockLiteral[] = 
"      █        █\n" 
" ██  ██  ██   ██\n" 
"██   █    ██  █ \n" 
"                \n"  
" █        █   █ \n" 
" █  ████  █   █ \n" 
" █        █   █ \n" 
" █        █   █ \n" 
"                \n"  
"      █       █ \n"
"██   ██   ██  ██\n"
" ██  █   ██    █\n"
"                \n"
"█   ███  ██    █\n"   
"█   █     █  ███\n" 
"██        █     \n"; 

int main(){
	setlocale(LC_ALL,"");
	initscr();
	noecho();
	raw();
	curs_set(0);
	//printw("%s",blockLiteral);
	
	block newBlock = {0};
	newBlock.type = O;
	
	timeout(1000);
	move(0,0);
	int initDrawingY = 0;
	for(;;){
		erase();
		for(int row = 0; row < 4; row++){
			int moved = 0;
			for(int column = 0; column < 3; ++column){
				char point = blockGraphics[newBlock.type][row*16 + column];
				if(point != 'X') {
					printw("%s","█");
					moved = 1;
				}
			}
			if(moved) move(initDrawingY++,0);
		}
		int input = getch();   	
		if(input == 'e') break;
		
	}

	getch();
	endwin();


	return 0;

}
