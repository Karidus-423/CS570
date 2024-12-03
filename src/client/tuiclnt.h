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

typedef struct Vec2 {
  int x;
  int y;
} Vec2;

typedef struct Win {
  Vec2 pos;
  Vec2 size;
  int id;
} Win;

int Open(char *filename_to_open, CLIENT **clnt, char *usr_name);
int Write(int fd, char *bfr, CLIENT **clnt, int bytes, char *usr_name);
void Close(int fd, CLIENT **clnt);
int Read(int fd, CLIENT **clnt, char *bfr, int numbytes, char *usr_name);
int Seek(int fd, int offset, CLIENT **clnt, char *usr_name);
void Delete(char *file_name, char *usr_name, CLIENT **clnt);
void List(char *usr_name, CLIENT **clnt);

#endif
