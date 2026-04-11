#include <stdlib.h>
#include <ncurses.h>

int main(int argc, char **argv){
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
	printw("\nPress anykey to continue...");
	getch();
	endwin();
	
	return 0;

}


	/*initscr();
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

