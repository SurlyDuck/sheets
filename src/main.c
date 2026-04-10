#include <stdlib.h>
#include <ncurses.h>

int main(int argc, char *argv){
	initscr();
	printw("Hey ncurses!!!"); //write to buffer, ncurses is not immediate
	refresh();
	getch();
	endwin();


	return 0;
}
