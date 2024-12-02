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

	char input[200] = {0};
	int input_pos = 0;

    // -----------------------------BEGIN TUI-----------------------------------
    initscr();
    cbreak();
    noecho();
	curs_set(0);

	//Center
	getmaxyx(stdscr,cy,cx);
    int starty = (cy - win_size.y) / 2;
    int startx = (cx - win_size.x) / 2;

    main_win = newwin(win_size.y, win_size.x, starty, startx);
    box(main_win, 0, 0);
    mvwprintw(main_win, 0, 2, server_dir);

	//Subwindows
	WINDOW* menu = subwin(main_win, menu_opt.size.y, menu_opt.size.x,
					      starty + 2, startx + 2);
	WINDOW* content = subwin(main_win, cont_opt.size.y, cont_opt.size.x, 
						  starty + 5, startx + 2);                    
	WINDOW* list = subwin(main_win, 27, 20, starty + 2, startx + 58);                    

    box(menu, 0, 0);
	box(content, 0, 0);
	box(list, 0, 0);

    mvwprintw(menu, 0, 2, "CMD");
    mvwprintw(content, 0, 2, "EXEC");
    mvwprintw(list, 0, 2, "FILES");

	int ch;
	while (true){
		wrefresh(main_win);
		mvwprintw(menu, 1, 2, ">");
		wrefresh(menu);

		ch = wgetch(menu);
		if ( ch == '\n'){
			werase(content);
			box(content, 0, 0);
			mvwprintw(content, 0, 2, "EXEC");
			input[input_pos] = '\0';

			HandleInput(input,content,client,usr_name);

			memset(input, 0, sizeof(input));
            input_pos = 0;
			werase(menu);
			box(menu, 0, 0);
			mvwprintw(menu, 0, 2, "INPUT");
            wrefresh(menu);
		}else if ( ch == KEY_BACKSPACE || ch == 127){
			if (input_pos > 0){
				input[--input_pos] = '\0';
				mvwprintw(menu, 1, 3 + input_pos, " "); 
                wmove(menu, 1, 3 + input_pos); 
			}
		}else if (input_pos < sizeof(input) - 1){
			input[input_pos++] = ch;
			mvwaddch(menu, 1, 3 + input_pos - 1, ch); 
		}
		wrefresh(menu);
	}


    delwin(menu);
    delwin(content);
    delwin(list);
    endwin();

    // ------------------------------END TUI------------------------------------
    return;
}


void HandleInput(char* cmd,WINDOW* content,CLIENT** clnt, char* username){
	char* tokens[5];
	char* after;
	int i = 0;

	char *token = strtok_r(cmd, ",", &after);
    while (token != NULL && i < 5) {
        tokens[i++] = token;  
        token = strtok_r(NULL, ",", &after); 
    }

	char* exec = tokens[0];
	char* filename;
	char* bfr;
	int fd;
	int bytes;

	int action = -1;
    if (strcmp(exec, "Open") == 0) {
        action = 0;
		filename = tokens[1];
    } else if (strcmp(exec, "Write") == 0) {
        action = 1;
		fd = atoi(tokens[1]);
		bfr = tokens[2];
		bytes = atoi(tokens[3]);
    } else if (strcmp(exec, "Close") == 0) {
        action = 2;
		fd = atoi(tokens[1]);
    } else if (strcmp(exec, "Read") == 0) {
        action = 3;
		fd = atoi(tokens[1]);
		bytes = atoi(tokens[2]);
    } else if (strcmp(exec, "Seek") == 0) {
        action = 4;
		fd = atoi(tokens[1]);
		bytes = atoi(tokens[2]);
    } else if (strcmp(exec, "Delete") == 0) {
        action = 5;
		filename = tokens[1];
		bfr = tokens[2];
    }

	switch (action){
		case 0: //Ask for filename
			mvwprintw(content, 1,2,"Open File %s",filename);
			int open_fd = Open(filename,clnt,username);
			mvwprintw(content, 1,4,"RETURN FD: %d",open_fd);
			break;
		case 1:// Ask for fd, bfr, bytes
			mvwprintw(content, 1,2,"Write File");
			break;
		case 2://Ask for fd
			mvwprintw(content, 1,2,"Close File");
			break;
		case 3://Askfor fd, bytes
			mvwprintw(content, 1,2,"Read File");
			break;
		case 4://Ask for fd, bytes
			mvwprintw(content, 1,2,"Seek File");
			break;
		case 5://Ask for filename, username
			mvwprintw(content, 1,2,"Delete File");
			break;
		default:
			mvwprintw(content, 1,2,"Incorrect Command");
			break;
	}

	wrefresh(content);
}

