#include "curses.h"
#include "ncurses.h"
#include "tuiclnt.h"

void FileServer(char* usr_name, char *host, CLIENT **client) {
    char server_dir[50];
    strcpy(server_dir, usr_name);
    strcat(server_dir, "@");
    strcat(server_dir, host);
    WINDOW *main_win;
	int cy,cx;
    Vec2 win_size = {80, 30};

    Win menu_opt, cont_opt, search_opt, list_opt;
    menu_opt.id = 1;
    menu_opt.pos = (Vec2){55, 3};
    menu_opt.size = (Vec2){55, 3};

    cont_opt.id = 1;
    cont_opt.pos = (Vec2){55, 4};
    cont_opt.size = (Vec2){55, 24};

    // -----------------------------BEGIN TUI-----------------------------------
    initscr();
    cbreak();
    noecho();

	//Center
	getmaxyx(stdscr,cy,cx);
    int starty = (cy - win_size.y) / 2;
    int startx = (cx - win_size.x) / 2;

    main_win = newwin(win_size.y, win_size.x, starty, startx);
    box(main_win, 0, 0);
    mvwprintw(main_win, 0, 2, server_dir);

	//Menu
	WINDOW* menu = subwin(main_win, menu_opt.size.y, menu_opt.size.x,
					      starty + 2, startx + 2);
    box(menu, 0, 0);
    mvwprintw(menu, 0, 2, "MENU");
	//Content
	WINDOW* content = subwin(main_win, cont_opt.size.y, cont_opt.size.x, 
					        starty + 5, startx + 2);                    
    box(content, 0, 0);
    mvwprintw(content, 0, 2, "CONTENT");
	//List
	WINDOW* list = subwin(main_win, 27, 20, 
					        starty + 2, startx + 58);                    
    box(list, 0, 0);
    mvwprintw(list, 0, 2, "List");

    wrefresh(menu);
    wrefresh(content);
    wgetch(main_win);

    delwin(menu);
    delwin(content);
    endwin();

    // ------------------------------END TUI------------------------------------
    return;
}

