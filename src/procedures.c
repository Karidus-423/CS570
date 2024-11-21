#include "genrpc/ssnfs.h"
#include <malloc.h>
#include <ncurses.h>
#include <rpc/clnt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    initscr();
    noecho();
    curs_set(0);

    int yMax, xMax;

    getmaxyx(stdscr, yMax, xMax);

    WINDOW *win = newwin(yMax / 2, xMax / 2, yMax / 4, xMax / 4);
    box(win, 0, 0);
    wgetch(win);
    endwin();
    return;
}

int main(int argc, char *argv[]) {
    CLIENT *client;
    char *host;
    if (argc < 2) {
        printf("Usage: %s hostname\n Flags:-d | Debug Mode\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "-d") == true) {
        // Enter Debug Mode for testing.
        host = argv[2];
    }

    host = argv[1];
    // Test Connection
    if (ConnectToServer(host, client) == false) {
        exit(EXIT_FAILURE);
    } else {
        FileServer(host, client);
    }
}
