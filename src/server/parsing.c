#include "serve.h"
#include <stdio.h>
#include <string.h>
#include <regex.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>


void process_line(const char *key, const char *value, int fd) {
    if (strcmp(key, "USER") == 0) {
	    printf("\nFound USER: %s\n", value);
		printf("Found FD: %d\n",fd);
        strcpy(files[fd].user, value);
    } else if (strcmp(key, "NAME") == 0) {
        printf("Found NAME: %s\n", value);
        strcpy(files[fd].name, value);
    } else if (strcmp(key, "FILE_START") == 0) {
        printf("Found FILE_START: %s\n", value);
        files[fd].file_start = atol(value);
    } else if (strcmp(key, "FILE_STATE") == 0) {
        printf("Found FILE_STATE: %s\n", value);
        files[fd].state = atoi(value); 
    } else if (strcmp(key, "FILE_INFO") == 0) {
        printf("Found FILE_INFO: %s\n", value);
        files[fd].block_info = atoi(value); 
    } else if (strcmp(key, "TEXT_START") == 0) {
        printf("Found TEXT_START: %s\n", value);
        files[fd].txt_start = atol(value);
    } else if (strcmp(key, "FILE_PTR") == 0) {
        printf("Found FILE_PTR: %s\n", value);
        files[fd].file_ptr = atol(value);
    }
}

void ScanDbToTable() {
    printf("Scanning DB for metadata...\n");

    FILE *db_file = fopen(DB_PATH, "rb");
    if (db_file == NULL) {
        perror("Failed to open file");
        return;
    }

	// files = malloc(initial_size * sizeof(fileMeta));
	files = calloc(initial_size, sizeof(fileMeta));
	if (files == NULL) {
		perror("Failed to initialize table\n");
		fclose(db_file);
	}else{
		printf("Initialized File Table.. Parsing DB.");
	}

    // Find file size
    fseek(db_file, 0, SEEK_END);
    long db_file_size = ftell(db_file);
    fseek(db_file, 0, SEEK_SET);

    long current_position = 0;
    char buffer[BUFFER_SIZE];

    // Compile regex
    regex_t regex;
    regmatch_t matches[3]; // To capture key and value
    const char *pattern = "_([A-Z_]+):([^ \n]+)";
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Failed to compile regex.\n");
        fclose(db_file);
        return;
    }

    // Read file in chunks and process
    while (current_position < db_file_size) {
        size_t bytes_to_read = (db_file_size - current_position > BUFFER_SIZE) ? BUFFER_SIZE : (db_file_size - current_position);
        size_t bytes_read = fread(buffer, 1, bytes_to_read, db_file);

        if (bytes_read <= 0) {
            perror("Error reading file");
            break;
        }

        buffer[bytes_read] = '\0'; // Null-terminate for safety

        const char *cursor = buffer;
        while (regexec(&regex, cursor, 3, matches, 0) == 0) {
            // Extract the key
            char key[50];
            snprintf(key, matches[1].rm_eo - matches[1].rm_so + 1, "%s", cursor + matches[1].rm_so);

            // Extract the value
            char value[50];
            snprintf(value, matches[2].rm_eo - matches[2].rm_so + 1, "%s", cursor + matches[2].rm_so);

            // Process the key-value pair
			int fd;
			if (strcmp(key, "FD") == 0) {
				// printf("Found FD: %s\n", value);
				fd = atoi(value);
				if (fd >= initial_size) {
					fprintf(stderr, "FD %d exceeds array size\n", fd);
					return;
				}
				files[fd].fd = fd; 
			}
            process_line(key, value,fd);

            // Move the cursor forward
            cursor += matches[0].rm_eo;
        }

        // Move file pointer to next block
        current_position += bytes_read;
    }

    // Free resources
    regfree(&regex);
    fclose(db_file);
    printf("Finished scanning DB.\n");
}

