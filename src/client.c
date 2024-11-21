#include "genrpc/ssnfs.h"
#include <malloc.h>
#include <ncurses.h>
#include <pwd.h>
#include <rpc/clnt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool ConnectToServer(char *host, CLIENT *client) {
    client = clnt_create(host, SSNFSPROG, SSNFSVER, "udp");
    if (client == NULL) {
        clnt_pcreateerror("Unable to create client at ");
        printf("%s\n", host);
        return false;
    }
    return true;
}

void FileServer(char *host, CLIENT *client) {
    char server_dir[50];
    strcpy(server_dir, getpwuid(getuid())->pw_name);
    strcat(server_dir, "@");
    strcat(server_dir, host);

    /// -----------------------------BEGIN TUI----------------------------------
    initscr();
    noecho();
    curs_set(0);

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

int Open(char *filename_to_open, CLIENT *clnt) {
    open_output *result_1;
    open_input open_file_1_arg;
    strcpy(open_file_1_arg.user_name, (getpwuid(getuid()))->pw_name);
    strcpy(open_file_1_arg.file_name, filename_to_open);
    result_1 = open_file_1(&open_file_1_arg, clnt);
    if (result_1 == (open_output *)NULL) {
        clnt_perror(clnt, "call failed");
    }
    printf(" File name is %s\n", (*result_1).out_msg.out_msg_val);
    return ((*result_1).fd);
}

int main(int argc, char *argv[]) {
    CLIENT *client;
    char *host;
    if (argc < 2) {
        printf("Usage: %s hostname\nFlags:-d | Debug Mode\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "-d") == 0) {
        // Enter Debug Mode for testing.
        host = argv[2];
        if (ConnectToServer(host, client) == false) {
            exit(EXIT_FAILURE);
        }
        // --------------------------Test Code----------------------------------
        printf("DEBUG\n");
        int file_desc = Open("Test-File", client);
        printf("Returned File Descriptor %d", file_desc);
        // ---------------------------------------------------------------------
    } else {
        host = argv[1];
        if (ConnectToServer(host, client) == false) {
            exit(EXIT_FAILURE);
        } else {
            FileServer(host, client);
        }
    }
}
