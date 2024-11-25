#ifndef SERVE
#define SERVE

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gen/ssnfs.h"

typedef enum storageErr {
  FILE_ADDED = 0,
  FILE_FOUND,
  STORAGE_FULL,
  DATABASE_INITIATED,
} storageErr;

typedef enum fileState {
  FILE_ERROR = -1,
  FILE_OPEN,
  FILE_CLOSED,
} fileState;

typedef struct fileMeta {
  char name[25];
  char user[25];
  int fd;          // Index in Array
  long file_start; // Used for deleting file.
  long txt_start;  // Bound to writeable area. No txt_start empty block.
  long file_ptr;   // Start at TEXT_START cannot go before it.
  fileState state;
} fileMeta;

#define FILE_SEPARATOR "\x1C"
#define GROUP_SEPARATOR "\x1D"
#define UNIT_SEPARATOR "\x1F"
#define TEXT_START "\x02"
#define NULL_BYTE "\x00"

#define BUFFER_SIZE 256
#define BLOCK 512
#define FILE_BLOCK (BLOCK * 64)

extern const char *DB_PATH;

int SetupDB();
int FindFile(char usr_name[], char file_name[]);
int ExtractUsedBlocks(char usr_name[], char file_name[]);
int RetrieveDatabaseInfo();
int AddFile(char usr_name[], char file_name[], FILE *db_ptr);

#endif
