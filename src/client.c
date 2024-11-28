#include "gen/ssnfs.h"
#include <malloc.h>
#include <ncurses.h>
#include <pwd.h>
#include <rpc/clnt.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


bool ConnectToServer(char *host, CLIENT **client) {
    *client = clnt_create(host, SSNFSPROG, SSNFSVER, "udp");
    if (client == NULL) {
        clnt_pcreateerror("Unable to create client at ");
        printf("%s\n", host);
        return false;
    }
    return true;
}

void FileServer(char *host, CLIENT **client) {
    char server_dir[50];
    strcpy(server_dir, getpwuid(getuid())->pw_name);
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

int Open(char *filename_to_open, CLIENT **clnt, char *usr_name) {
	open_output *result_open;

	open_input open_file_1_arg;
	strcpy(open_file_1_arg.user_name, (getpwuid(getuid()))->pw_name);
	strcpy(open_file_1_arg.file_name, filename_to_open);

	result_open = open_file_1(&open_file_1_arg, *clnt);
	if (result_open == (open_output *)NULL) {
		clnt_perror(*clnt, "call:open failed ");
	}

	if (strcmp(filename_to_open, "01CheckStep") != 0){
		printf("File name is %s\n", (*result_open).out_msg.out_msg_val);
	}
	return ((*result_open).fd);
}

int Write(int fd, char *bfr, CLIENT **clnt, int bytes, char *usr_name) {
    write_output *write_result;

    write_input write_arg;

    // Validate inputs
    if (bfr == NULL || clnt == NULL || *clnt == NULL || usr_name == NULL) {
        fprintf(stderr, "Error: Invalid input to Write\n");
        return -1;
    }

	strcpy(write_arg.user_name,usr_name);
    write_arg.fd = fd;
    write_arg.numbytes = bytes;
    write_arg.buffer.buffer_val = strdup(bfr);
    write_arg.buffer.buffer_len = strlen(bfr);

    // Debug inputs
    // printf("Debug: fd=%d, bytes=%d, user_name=%s, buffer_len=%d, buffer_val=%s\n",
    //        fd, bytes, write_arg.user_name, write_arg.buffer.buffer_len, write_arg.buffer.buffer_val);

    // Make remote procedure call
    write_result = write_file_1(&write_arg, *clnt);
    if (write_result == NULL) {
        clnt_perror(*clnt, "RPC call failed");
        return -1;
    }

    // Handle success and error responses
    if (write_result->success != 0) {
        fprintf(stderr, "Write failed: %s\n", write_result->out_msg.out_msg_val ? write_result->out_msg.out_msg_val : "Unknown error");
        return -1;
    }

	printf("Write Success\n");
    return ((*write_result).success);
}

void Close(int fd, CLIENT **clnt){
	close_output *result_close;

	close_input close_file_1_arg;
	strcpy(close_file_1_arg.user_name, (getpwuid(getuid()))->pw_name);
	close_file_1_arg.fd = fd;

	result_close = close_file_1(&close_file_1_arg, *clnt);
	if (result_close == (close_output *)NULL) {
		clnt_perror(*clnt, "call open failed ");
	}
	printf("%s\n", (*result_close).out_msg.out_msg_val);

}

void Read(int fd, CLIENT **clnt, char *bfr, int numbytes,char *usr_name){
	read_output *read_result;

	read_input read_arg;
	strcpy(read_arg.user_name,usr_name);
	read_arg.fd = fd;
	read_arg.numbytes = numbytes;

	read_result = read_file_1(&read_arg, *clnt);
	if (read_result == (read_output *)NULL) {
		clnt_perror(*clnt, "call open failed ");
	}
	printf("%s\n", (*read_result).out_msg.out_msg_val);

}

void Test(CLIENT **clnt, char *usr_name){
	// printf("TEST\n");
	int i,j;
	int fd1,fd2;
	int wrt_result;
	char buffer[100];
	fd1=Open("File1",clnt,usr_name); 
	printf("Returned File Descriptor %d\n", fd1);
	for (i=0; i< 20;i++){
		wrt_result = Write(fd1,  "This is a test program for cs570 assignment 4", clnt, 15,usr_name);
		if (wrt_result != 0){
			printf("Unable to write to file.\n");
		}
	}
	Close(fd1, *&clnt);

	fd2=Open("File2",*&clnt,usr_name);
	for (i=0; i< 20;i++){
		wrt_result = Write(fd2,  "for cs570 assignment 4", clnt, 15,usr_name);
		if (wrt_result != 0){
			printf("Unable to write to file.\n");
		}
	}
	// for (j=0; j< 20;j++){
	Read(fd2,clnt, buffer, 10,usr_name);
	printf("Read:%s\n",buffer);
	// }
	// Seek (fd2,40);
	// Read(fd2, buffer, 20);
	// printf("%s\n",buffer);
	// Close(fd2,*&clnt);
	// Delete("File1");
	// List();
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
        if (ConnectToServer(host, &client) == false) {
            exit(EXIT_FAILURE);
        }

        // --------------------------Test Code----------------------------------
		char *usr_name = getpwuid(getuid())->pw_name;
		int fd = Open("01CheckStep",&client,usr_name);
		Test(&client,usr_name);
        // ---------------------------------------------------------------------
    } else {
        host = argv[1];
        if (ConnectToServer(host, &client) == false) {
            exit(EXIT_FAILURE);
        } else {
            FileServer(host, &client);
        }
    }
}
