#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gen/ssnfs.h"



typedef enum fileState { 
	FILE_OPEN,
	FILE_CLOSED,
	FILE_ERROR,
} fileState;


#define FILE_SEPARATOR 0x1C
#define UNIT_SEPARATOR 0x1F
#define TEXT_START 0x02
#define BLOCK 512
#define MAX_SIZE (16 * 1024 * 1024) //16MB
#define MAX_BLOCKS (MAX_SIZE/BLOCK)

typedef unsigned int block;
typedef struct fileMeta{
	char user[20];
	char name[20];
	int	fd; 
	block size;
	block used;
	fileState state;
} fileMeta;

static int setup_result = 0;
const char *db_dir = "./src/database.dt";

int SetupDB();


int FindFile(char usr_name[], char file_name[]) {

	return 0; 
}

int AddFile(char usr_name[], char file_name[]) {
	/*FILENAME:"filename"
	 *---------------------
	 *user:"username"
	 *fd:number
	 *size: number of blocks
	 *used: amount text
	 *state: Open or closed
	 *---------------------
	*/

	FILE *db_ptr = fopen("./src/datbase.dt", "w");
	if (db_ptr == NULL){
		printf("Failed to open database.\n");
		SetupDB();// Initialize Database
		return 0;
	}

	int curr_byte;
	long last_known_file = -1;

	// Read until EOF
    while ((curr_byte = fgetc(db_ptr)) != EOF) {
		// Get current file position
        long current_pos = ftell(db_ptr);  
        // Check if current byte is the separator or if the next byte is a null byte
        if (curr_byte == FILE_SEPARATOR) {
            // Look ahead to the next byte
            int next_byte = fgetc(db_ptr);
            if (next_byte == EOF || next_byte == '\0') {
                last_known_file = current_pos;
            }
            // Return the file pointer to its previous position after checking
            ungetc(next_byte, db_ptr);
        }
    }

	return 0; 
}

int ExtractUsedBlocks(char usr_name[], char file_name[]) {

	return 0; 
}

int RetrieveDatabaseInfo(){
	return 0;
}

int SetupDB(){
	struct stat st ={0};
	int check_dr = stat(db_dir,&st);

	if(check_dr != 0){
		int mk_dir = mkdir(db_dir, 0700);
		if (mk_dir != 0){
			fprintf(stderr, "ERROR: %s\n",strerror(errno));
			exit(0);
		}
		printf("Database not found. Database created at %s",db_dir);
		//Initialize database.dt
		//Set db_dir to have a max size of 16MB. Fill with null values.
		int db_desc = open("db_dir", O_RDWR|O_CREAT,0600);
		if (db_desc < 0){
			fprintf(stderr, "ERROR: %s\n",strerror(errno));
			exit(0);
		}
		off_t current_size = st.st_size;

			// If the file is smaller than 16 MB, extend it
		if (current_size < MAX_SIZE) {
			if (lseek(db_desc, MAX_SIZE - 1, SEEK_SET) < 0) {
				fprintf(stderr, "ERROR: Failed to seek in database file '%s': %s\n", db_dir, strerror(errno));
				close(db_desc);
				exit(EXIT_FAILURE);
			}

			// Write a single NULL byte at the end
			if (write(db_desc, "\0", 1) != 1) {
				fprintf(stderr, "ERROR: Failed to extend database file '%s': %s\n", db_dir, strerror(errno));
				close(db_desc);
				exit(EXIT_FAILURE);
			}

			printf("Database file '%s' initialized to 16 MB.\n", db_dir);
		} else {
			printf("Database file '%s' is already initialized.\n", db_dir);
		}
		close(db_desc);
		setup_result = 0;
		return setup_result; //0 = Needed to init database. Data Empty.
	}

	setup_result = 1;
	return setup_result; //1 = Did not need to init database. Data Present
}

open_output *open_file_1_svc(open_input *argp, struct svc_req *rqstp) {
    static open_output result;
	//Check if database is present. 
	if (strcmp(argp->file_name, "01CheckStep")== 0){
		if (SetupDB() == 1){
			RetrieveDatabaseInfo();
		}
		return &result;
	}

	fileMeta usr_file;
	usr_file.state = FILE_CLOSED;
	strcpy(usr_file.user ,argp->user_name);
	strcpy(usr_file.name , argp->file_name);

	if (FindFile(usr_file.user, usr_file.name) == 0) {
        usr_file.state = FILE_OPEN;
        usr_file.fd = atoi(usr_file.user) + atoi(usr_file.name);
	}else{
		usr_file.fd = atoi(usr_file.user) + atoi(usr_file.name);
		AddFile(usr_file.user, usr_file.name);
		usr_file.state = FILE_OPEN;
	}

	usr_file.size = MAX_SIZE;
	usr_file.used = ExtractUsedBlocks(usr_file.user, usr_file.name);

	result.fd = usr_file.fd;
	result.out_msg.out_msg_len = 10;
	result.out_msg.out_msg_val =
		(char *)malloc(result.out_msg.out_msg_len + 1);
    strcpy(result.out_msg.out_msg_val, (*argp).file_name);
    printf("In server: filename recieved:%s\n", argp->file_name);
	free(result.out_msg.out_msg_val);

    printf("In server username received:%s\n", argp->user_name);
    return &result;
}

read_output *read_file_1_svc(read_input *argp, struct svc_req *rqstp) {
    static read_output result;

    return &result;
}

write_output *write_file_1_svc(write_input *argp, struct svc_req *rqstp) {
    static write_output result;
	result.out_msg.out_msg_len = 10;
	free(result.out_msg.out_msg_val);

    return &result;
}

list_output *list_files_1_svc(list_input *argp, struct svc_req *rqstp) {
    static list_output result;

    return &result;
}

delete_output *delete_file_1_svc(delete_input *argp, struct svc_req *rqstp) {
    static delete_output result;

    return &result;
}

close_output *close_file_1_svc(close_input *argp, struct svc_req *rqstp) {
    static close_output result;

    return &result;
}

seek_output *seek_position_1_svc(seek_input *argp, struct svc_req *rqstp) {
    static seek_output result;

    return &result;
}
