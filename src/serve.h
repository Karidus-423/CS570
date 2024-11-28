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
  EMPTY,
  HAS_CONTENT,
} fileState;

typedef struct fileMeta {
  char name[25];
  char user[25];
  int fd;          // Index in Array
  long file_start; // Used for deleting file.
  long txt_start;  // Used to if file_ptr out of bounds.
  long file_ptr;   // Start at TEXT_START cannot go before it.
  fileState state;
  fileState block_info;
} fileMeta;

#define FILE_SEPARATOR "\x1C"
#define GROUP_SEPARATOR "\x1D"
#define UNIT_SEPARATOR "\x1F"
#define TEXT_START "\n\x02"
#define NULL_BYTE "\x00"

#define BLOCK 512
#define BUFFER_SIZE BLOCK
#define FILE_BLOCK (BLOCK * 64)
#define MAX_DB_SIZE (16 * 1024 * 1024) // 16Mb

extern const char *DB_PATH;
extern int initial_size;
extern fileMeta *files; // Dynamic array to hold file metadata

int SetupDB();
int FindFile(char usr_name[], char file_name[]);
int AddFile(char usr_name[], char file_name[]);
int InitDBTable();
int SaveFileToDB(fileMeta file);
void ScanDbToTable();
int RetrieveDatabaseInfo();

#endif
