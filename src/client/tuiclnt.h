#ifndef TUICLIENT
#define TUICLIENT

#include "../gen/ssnfs.h"

#include <malloc.h>
#include <ncurses.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int Open(char *filename_to_open, CLIENT **clnt, char *usr_name);
int Write(int fd, char *bfr, CLIENT **clnt, int bytes, char *usr_name);
void Close(int fd, CLIENT **clnt);
int Read(int fd, CLIENT **clnt, char *bfr, int numbytes, char *usr_name);
int Seek(int fd, int offset, CLIENT **clnt, char *usr_name);
void Delete(char *file_name, char *usr_name, CLIENT **clnt);
void List(char *usr_name, CLIENT **clnt);
void FileServer(char *usr_name, char *host, CLIENT **client);

#endif
