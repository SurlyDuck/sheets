#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#define YOFFSET 5

WINDOW *gameWindow;
WINDOW *CreateWindow(int width, int height, int ix, int iy);
int playerPosX = 0;
int playerPosY = 0;

const char *gameMsg = "@@@@@ SNAKE! @@@@@";

int main(int argc, char **argv){
	int width, height, startx,starty = 0;

	initscr();
	noecho();
	raw();
	curs_set(0);
	getmaxyx(stdscr,height,width);
	attron(A_BOLD | A_BLINK);
	mvprintw(0, (width-strlen(gameMsg))/2,gameMsg);
	attroff(A_BOLD | A_BLINK);
	refresh();
	height /= 2;
	gameWindow = CreateWindow(width,height,startx,starty+YOFFSET);
	
	mvaddch(height/2+YOFFSET,width/2,'@');

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
/*
int mains3(){
	
	initscr();
	//addch(ACS_LANTERN | A_BOLD);	
	//addstr("\nthis is a string");
	//refresh();
	
	int x,y = 0;
	getmaxyx(stdscr,y,x);
	printw("ROWS: %d, COLUMNS: %d",y,x);
	
	//mvaddch(y/2,x/2,'b');

	getch();
	endwin();

	return 0;
}

int main2(int argc, char **argv){
	int cline, longestLine, lineLenght, longestLenght = 0;
	char cchar;
	initscr();
	noecho();
	raw();
	printw("Start typing!");
	refresh();
	
	while(cchar !=  '\e'){
		printw("\nLine %d:",cline);
		cchar = getch();
		refresh();
		while(cchar != '\n'){
			if(cchar == '\e') break;
			++lineLenght;
			attron(A_DIM);
			printw("%c",cchar);
			attroff(A_DIM);
			cchar = getch();
		}			
		
		if(lineLenght > longestLenght) {
			longestLine = cline;
			longestLenght = lineLenght;
		}
		++cline;
		lineLenght = 0;
	}	
	
	printw("\nLongest line: %d",longestLine);
	printw("\nPress any key to continue...");
	getch();
	endwin();
	
	return 0;

}


	initscr();
	noecho();	
	raw();
	printw("The next five characters will have underline:  \n");
	
	int n = 5;
	while(n --> 0){
	int character;
		character = getch();

		attron(A_UNDERLINE);
		printw("%c",character);
		attroff(A_UNDERLINE);
	}
	refresh();
	getch();
	endwin();
*/
