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

int FindFile(char usr_name[], char file_name[]) {
	return 0;
}


int AddFile(char usr_name[], char file_name[], FILE *db_ptr) {
	return 0;
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
	fwrite(null_bfr, 1,start_bfr,db_init);
	free(null_bfr);
	fclose(db_init);

    FILE *file_set = fopen(DB_PATH, "r+");
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
        // Write 'F' to the current position
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

int GetMeta(char *bfr){
	//TODO: GET DATA FROM GIVEN BFR.
	printf("Parsing...");
	return 0;
}

int InitDBTable() {
	if (files == NULL){
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
				files[file_found - 1].file_start = file_pos;
			}
		}

		printf("Files Found: %d\n", file_found);

		//Parsing File metadata.
		char file_state[2] = {'\0', '\0'};
		for (int i = 0; i < file_found; i++) {
			printf("File %d starts at position: %ld\n", i + 1, 
				   files[i].file_start);

			fseek(db_file, files[i].file_start + 1 ,SEEK_SET);
			file_state[0] = fgetc(db_file);
			if (strcmp(file_state,GROUP_SEPARATOR) == 0){
				char meta[BUFFER_SIZE];
				char temp[BUFFER_SIZE];
				memset(meta, 0, BUFFER_SIZE); // Initialize meta buffer

				while (1) {
					// Read into a temporary buffer
					if (fgets(temp, BUFFER_SIZE, db_file) == NULL) {
						// Handle EOF or error
						fprintf(stderr, "Error or EOF while reading metadata\n");
						break;
					}

					// Append temp to meta (if needed) or process directly
					strcat(meta, temp);

					// Check if TEXT_START is found
					if (strstr(meta, TEXT_START) != NULL) {
						// Stop reading as TEXT_START is found
						break;
					}
				}
				// Further processing of meta, if required
				printf("Metadata parsed: %s\n", meta);
			}
		}
		fclose(db_file);
		return 0;
	}else{
		printf("File Table Present.");
		return 0;
	}
}



int SetupDB(){
	int db_present = access(DB_PATH, F_OK);
	int init_table = 0;
	if (db_present != 0){
		printf("error = DB not present....\nTrying to init DB.\n");
		// fprintf(stderr,"error = %s\nTrying to init DB.\n",strerror(errno));
		//Initiate DB
		if (InitDBFile() != 0){
			printf("Database Creation Failed.\n");
			exit(EXIT_FAILURE);
		}else{
			printf("Database Creation Success... Initiating Table\n");
			InitDBTable();
		}
		return 0;
	}else{
		//Server Crashed/Restarted.
		printf("DB Present. Checking Table...\n");
		InitDBTable();
		return 0;
	}
}
