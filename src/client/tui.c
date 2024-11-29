#include "tuiclnt.h"

void FileServer(char* usr_name, char *host, CLIENT **client) {
    char server_dir[50];
    strcpy(server_dir, usr_name);
    strcat(server_dir, "@");
    strcat(server_dir, host);

    // -----------------------------BEGIN TUI-----------------------------------
	initscr(); noecho(); curs_set(0);

    int yMax, xMax;

    getmaxyx(stdscr, yMax, xMax);

    WINDOW *win = newwin(yMax / 2, xMax / 2, yMax / 4, xMax / 4);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, "%s", server_dir);
    mvwprintw(win, 2, 2, "> ");
    while (wgetch(win) != 'q') {
    };
    endwin();
    // ------------------------------END TUI------------------------------------ 
	return;
}
