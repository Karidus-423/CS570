#include "serve.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *DB_PATH = "./database.dt";
// Global Variables
fileMeta *files = NULL;     // Dynamic array to hold file metadata
int file_found = 0;         // Number of files found
int initial_size = 16;      // Initial capacity of the array

long GetFileSize(const char *filename) {
    FILE *file = fopen(filename, "rb"); // Open file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return -1; // Return -1 on error
    }

    // Move the file pointer to the end of the file
    fseek(file, 0, SEEK_END);

    // Get the current position of the file pointer (size of the file)
    long file_size = ftell(file);

    fclose(file);
    return file_size;
}


int FindFile(char usr_name[], char file_name[]) {
	for (int i = 0; i < file_found;i++){
		if ((strcmp(usr_name, files[i].user) == 0) &&
			(strcmp(file_name, files[i].name) == 0)) {

			files[i].state = FILE_OPEN;
			SaveFileToDB(files[i]);
			return files[i].fd;
		}
	}
	return -1;
}

int AddDbBlocks(const char *mode){
    FILE *file_set = fopen(DB_PATH, mode);
    if (!file_set) {
        perror("Error opening file");
        return 1;
    }

    long current_position = 0;
	int f_allocated = 0;
    
	// Find EOF
	fseek(file_set, 0, SEEK_END);
	long file_set_size = ftell(file_set);

    // Loop until EOF
    while (fseek(file_set, current_position, SEEK_SET) == 0) {
        if (fwrite(FILE_SEPARATOR, sizeof(char), 1, file_set) != 1) {
            perror("Error writing to file_set");
            fclose(file_set);
            return 1;
        }
		f_allocated++;

        // Move the file_set pointer by FILE_BLOCK bytes
        current_position += FILE_BLOCK;

        if (current_position >= file_set_size) {
            break;
        }
    }
	fclose(file_set);
	printf("%d\n",f_allocated);

    return 0;
}

char* BuildMetaStr(fileMeta file){
	static char meta_str[BUFFER_SIZE] = {'\0'};
	// Start with the GROUP_SEPARATOR
    snprintf(meta_str, sizeof(meta_str), "%s", GROUP_SEPARATOR);

    // Append fields, converting non-string types to strings
    snprintf(meta_str + strlen(meta_str), sizeof(meta_str) - strlen(meta_str),
			 "%s%s", UNIT_SEPARATOR, file.name);
    snprintf(meta_str + strlen(meta_str), sizeof(meta_str) - strlen(meta_str),
			 "%s%s", UNIT_SEPARATOR, file.user);
    snprintf(meta_str + strlen(meta_str), sizeof(meta_str) - strlen(meta_str),
			 "%s%d", UNIT_SEPARATOR, file.fd);
    snprintf(meta_str + strlen(meta_str), sizeof(meta_str) - strlen(meta_str),
			 "%s%ld", UNIT_SEPARATOR, file.file_start);

    snprintf(meta_str + strlen(meta_str), sizeof(meta_str) - strlen(meta_str),
			 "%s%ld", UNIT_SEPARATOR, file.txt_start);
    snprintf(meta_str + strlen(meta_str), sizeof(meta_str) - strlen(meta_str),
			 "%s%ld", UNIT_SEPARATOR, file.file_ptr);

    snprintf(meta_str + strlen(meta_str), sizeof(meta_str) - strlen(meta_str),
			 "%s%d", UNIT_SEPARATOR, file.state);
    snprintf(meta_str + strlen(meta_str), sizeof(meta_str) - strlen(meta_str),
			 "%s%d", UNIT_SEPARATOR, file.block_info);

    // snprintf(meta_str + strlen(meta_str), sizeof(meta_str) - strlen(meta_str),
    // "%s", TEXT_START);

	return meta_str;
}

int SaveFileToDB(fileMeta file){
	FILE *db_file = fopen(DB_PATH,"r+");
	if (!db_file) {
		perror("Error opening file");
		return 1;
	}

	char *meta_str = BuildMetaStr(file);
	printf("%s\n",meta_str);
	//Write file meta to db.
	fseek(db_file, file.file_start, SEEK_SET);
	fwrite(meta_str, BUFFER_SIZE, 1, db_file);
	fseek(db_file, file.txt_start, SEEK_SET);
	fwrite(TEXT_START, 1, 1, db_file);
	fclose(db_file);
	return 0;
}

int SaveTableToDB(){
	FILE *db_file = fopen(DB_PATH,"r+");
	if (!db_file) {
		perror("Error opening file");
		return 1;
	}
	for(int i = 0; i < file_found; i++){
		char *meta_str = BuildMetaStr(files[i]);
		printf("%s\n",meta_str);
		//Write file meta to db.
		fseek(db_file, files[i].file_start, SEEK_SET);
		fwrite(meta_str, BUFFER_SIZE, 1, db_file);
		fseek(db_file, files[i].txt_start, SEEK_SET);
		fwrite(TEXT_START, 1, 1, db_file);
	}
	fclose(db_file);
	return 0;
}


int ScanDbToTable(){
	printf("Entering ScanDBToTable");
	//Allocate 100KB more to the db file. Continue adding file_sept from last.
	size_t start_bfr = 100 * 1024;
	// Open the file for writing
	FILE *db_init = fopen(DB_PATH, "a+");
	if (!db_init) {
		perror("Error opening file");
		return 1;
	}
	//Intiate 100kb of null. 
	unsigned char *null_bfr = (unsigned char *)malloc(start_bfr);
	if (null_bfr == NULL){
		perror("Failed to allocate null bfr.");
		return 1;
	}

	//Append 100kb of null.
	fwrite(null_bfr, 1,start_bfr,db_init);
	free(null_bfr);
	fclose(db_init);

	//Initialize new 100kb with FILE_SEPARATOR every 64 blocks.
	int new_blocks = AddDbBlocks("r+");
	if (new_blocks == 0){
		//Blocks added successfully. Rebuild DbTable.
		if (SaveTableToDB() == 0){
			//Saved Success. Refresh table with new file blocks.
			//Call AddFile again as we have more space to fill.
			// AddFile(usr_name, file_name);
		}else{
			printf("Saving Current Table to DB Failed");
			return 1;
		}
	}
	return 0;
}


int AddFileSpace(){
	printf("File Full Increasing Capacity");
	return 0;
}
int AddFile(char usr_name[], char file_name[]) {
	//Look For empty file block.
	int empty_block = -1;
	for (int i = 0; i < file_found;i++){
		if (files[i].block_info == EMPTY){
			strcpy(files[i].name,file_name);
			strcpy(files[i].user,usr_name);
			files[i].block_info = HAS_CONTENT;
			files[i].state = FILE_OPEN;
			empty_block = files[i].fd;
			SaveFileToDB(files[i]);
			return empty_block;
		}
	}

	if (empty_block == -1){
		//No empty block. Check current db space.
		int file_size = GetFileSize(DB_PATH);
		if (file_size >= MAX_DB_SIZE){
			printf("MAX STORAGE REACHED. DELETE FILES.\n");
			return -1;
		}else{
			//Space Available Allocate more space.
			AddFileSpace();
		}
	}
	return 1;
	
}

int InitDBFile(){
 // Open the file for writing
    FILE *db_init = fopen(DB_PATH, "wb");
    if (!db_init) {
        perror("Error opening file");
        return 1;
    }
	//Intiate 500kb of null. 
	size_t start_bfr = 500 * 1024;
	unsigned char *null_bfr = (unsigned char *)malloc(start_bfr);
	if (null_bfr == NULL){
		perror("Failed to allocate null bfr.");
		return 1;
	}

	//Write 500kb of null.
	if(fwrite(null_bfr, 1,start_bfr,db_init) != 0){
		free(null_bfr);
	}
	fclose(db_init);

	if (AddDbBlocks("r+") == 0){
		if (InitDBTable() == 0){
			for (int i = 0; i < file_found;i++){
				SaveTableToDB();
			}
		}else{
			printf("Unable to create table");
		}
	}else{
		printf("Unable to write to file.");
	}

    return 0;
}

int InitDBTable() {
	if (files == NULL){
		//Parse InitDB and fill files with defaults
		FILE *db_file = fopen(DB_PATH, "r");
		if (db_file == NULL) {
			perror("Failed to open file");
			return -1;
		}

		// Allocate initial memory for files array
		files = malloc(initial_size * sizeof(fileMeta));
		if (files == NULL) {
			perror("Failed to initialize table\n");
			fclose(db_file);
			return -1;
		}

		char ch;
		char str[2] = {'\0', '\0'};
		long file_pos = 0;

		while ((ch = fgetc(db_file)) != EOF) {
			str[0] = ch;
			// Increment file position
			file_pos++;
			if (strcmp(str, FILE_SEPARATOR) == 0) {
				file_found++;
				// Resize array if needed
				if (file_found > initial_size) {
					initial_size *= 2;
					fileMeta *new_files = 
						realloc(files, initial_size * sizeof(fileMeta));
					if (new_files == NULL) {
						perror("Failed to reallocate memory");
						free(files);
						fclose(db_file);
						return -1;
					}
					files = new_files;
				}
				// Store the current position
				int i = file_found -1;
				files[i].file_start = file_pos;
				files[i].fd = i;
				//Init with defaults
				strcpy(files[i].name,"No Name");
				strcpy(files[i].user,"No User");
				files[i].state = FILE_CLOSED;
				files[i].txt_start = files[i].file_start + BUFFER_SIZE;
				files[i].file_ptr = files[i].txt_start;
		 		files[i].block_info = EMPTY;
			}
		}
		printf("Files created in table: %d\n", file_found);
		return 0;
	}else{
		//Nothing to do.
		printf("File Table Present.");
		return 0;
	}
}



int SetupDB(){
	int db_present = access(DB_PATH, F_OK);
	if (db_present != 0){
		printf("error = DB not present....\nTrying to init DB.\n");
		// fprintf(stderr,"error = %s\nTrying to init DB.\n",strerror(errno));
		//Initiate DB
		if (InitDBFile() != 0){
			printf("Database Creation Failed.\n");
			exit(EXIT_FAILURE);
		}
		return 0;
	}else{
		//Server Crashed/Restarted.
		printf("DB Present. Checking Table...\n");
		if (files == NULL){
			ScanDbToTable();
		}else{
			printf("Table Present.");
		}
		return 1;
	}
}
