#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>

char blocks[] = {
"      █        █\n" 
" ██  ██  ██   ██\n" 
"██   █    ██  █ \n" 
"                \n"  
" █        █   █ \n" 
" █  ████  █   █ \n" 
" █        █   █ \n" 
" █        █   █ \n" 


};
int main(){
	setlocale(LC_ALL,"");
	initscr();
	curs_set(0);
	printw("%s",blocks);

	getch();
	endwin();


	return 0;

}
