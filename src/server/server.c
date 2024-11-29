#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "serve.h"

//Should only update once per lifetime. As all clients send "01CheckStep".
int lifetime_checks = 0;

void UpdateWriteResult(write_output *result, const char *msg, int success) {
  result->out_msg.out_msg_len = strlen(msg);
  free(result->out_msg.out_msg_val);
  result->out_msg.out_msg_val = strdup(msg);
  result->success = success;
}

void UpdateReadResult(read_output *result, const char *msg, int success) {
  result->out_msg.out_msg_len = strlen(msg);
  free(result->out_msg.out_msg_val);
  result->out_msg.out_msg_val = strdup(msg);
  result->success = success;
}

void UpdateDeleteResult(delete_output *result, const char *msg) {
  result->out_msg.out_msg_len = strlen(msg);
  free(result->out_msg.out_msg_val);
  result->out_msg.out_msg_val = strdup(msg);
}

void UpdateListResult(list_output *result, char *msg) {
  result->out_msg.out_msg_len = strlen(msg);
  free(result->out_msg.out_msg_val);
  result->out_msg.out_msg_val = strdup(msg);
}

open_output *open_file_1_svc(open_input *argp, struct svc_req *rqstp) {
	static open_output open_result;
	// Check if database is present.
	if ((strcmp(argp->file_name, "01CheckStep") == 0)) {
		printf("Check Step Recieved.\n");
		if(lifetime_checks == 0){
			SetupDB();
			lifetime_checks++;
		}else{
			printf("Ignore Check step");
			return &open_result;
		}
		return &open_result;
	}else{
		open_result.fd = FindFile(argp->user_name, argp->file_name);
		if (open_result.fd == -1) {
			// File not found.
			if (AddFile(argp->user_name, argp->file_name) == 0) {
				// Added File
				open_result.fd = FindFile(argp->user_name, argp->file_name);
				open_result.out_msg.out_msg_len = 10;
				free(open_result.out_msg.out_msg_val);
				open_result.out_msg.out_msg_val =
					(char *)malloc(open_result.out_msg.out_msg_len);
				strcpy(open_result.out_msg.out_msg_val, (*argp).file_name);
				printf("In server: filename recieved:%s\n", argp->file_name);
				printf("In server username received:%s\n", argp->user_name);
			} else {
				printf("Unable to add file\n");
				exit(EXIT_FAILURE);
			}
		} else {
			open_result.out_msg.out_msg_len = 10;
			free(open_result.out_msg.out_msg_val);
			open_result.out_msg.out_msg_val =
				(char *)malloc(open_result.out_msg.out_msg_len);
			strcpy(open_result.out_msg.out_msg_val, (*argp).file_name);
			printf("In server: filename recieved:%s\n", argp->file_name);
			printf("In server username received:%s\n", argp->user_name);
		}
		return &open_result;
	}
}

read_output *read_file_1_svc(read_input *argp, struct svc_req *rqstp) {
	static read_output result;
	int fd = argp->fd;
	char read_bfr[50] ={'0'};

	FILE *fp = fopen(DB_PATH,"r");

	fseek(fp, files[fd].file_ptr, SEEK_SET);
	if (fread(read_bfr, argp->numbytes,1, fp) != argp->numbytes){
		printf("Error Reading file.\n");
	}
	files[fd].file_ptr = ftell(fp);

	fclose(fp);
	SaveFileToDB(files[fd]);

	result.buffer.buffer_len = argp->numbytes;
	result.buffer.buffer_val = strdup(read_bfr);

	UpdateReadResult(&result, "Read Success", 0);
	return &result;
}

write_output *write_file_1_svc(write_input *argp, struct svc_req *rqstp) {
    static write_output write_result;
    int fd = argp->fd;
    fileMeta *file = &files[fd];
	printf("Debug: Writing %u bytes from buffer: '%s'\n", 
	argp->buffer.buffer_len, argp->buffer.buffer_val);


    int found_fd = FindFile(argp->user_name, file->name);
    if (found_fd == fd) {
        if (file->state == FILE_OPEN && file->fd == argp->fd) {
            FILE *fp = fopen(DB_PATH, "r+");
            if (fp == NULL) {
                perror("Error opening DB");
                UpdateWriteResult(&write_result, "Error Opening", -1);
                return &write_result;
            }
			// printf("Debug: Seeking to position %ld in file\n", file->file_ptr);
			if (fseek(fp, file->file_ptr, SEEK_SET) != 0) {
				perror("Error seeking DB");
				fclose(fp);
				UpdateWriteResult(&write_result, "Error Seeking", -1);
				return &write_result;
			}
			if (fwrite(argp->buffer.buffer_val,argp->numbytes,1,  fp) == 0) {
                perror("Error writing DB");
				printf("Debug: Writing %u bytes from buffer: %s\n", 
				   argp->numbytes, argp->buffer.buffer_val);
                fclose(fp);
                UpdateWriteResult(&write_result, "Error Writing", -1);
                return &write_result;
            }
            file->file_ptr = ftell(fp);
			// fseek(fp, file->txt_start, SEEK_SET);
			// char buffer[30];
			// fread(buffer, 1, 30, fp);
			// buffer[29] = '\0';  // Null-terminate
			// printf("File contents after write: %s\n", buffer);
            fclose(fp);
			SaveFileToDB(*file);

            UpdateWriteResult(&write_result, "Write Success", 0);
        } else {
            printf("File not open.");
            UpdateWriteResult(&write_result, "File Closed", -1);
        }
    } else {
        printf("File not found. Please create by opening.");
        UpdateWriteResult(&write_result, "Write Failure", -1);
    }
    return &write_result;
}

list_output *list_files_1_svc(list_input *argp, struct svc_req *rqstp) {
	static list_output result;
	char files_found[BUFFER_SIZE] = {0};

	for (int i = 0; i < initial_size; i++){
		int found = strcmp(files[i].user, argp->user_name);
		if (found == 0){
			//Found File under username
			strcat(files_found, files[i].name);
			strcat(files_found, "\n");
		}
	}

	UpdateListResult(&result,files_found);
	// printf("%s\n",result.out_msg.out_msg_val);

	return &result;
}

delete_output *delete_file_1_svc(delete_input *argp, struct svc_req *rqstp) {
    static delete_output result;
	fileMeta* file;

	int fd = FindFile(argp->user_name, argp->file_name);
	if (fd != -1){
		file = &files[fd];
	}else{
		printf("Error Deleting: File not found\n");
	}

	//Find Size and locations of file.
	long size = FILE_BLOCK - BLOCK;
	//Clear Text Blocks
	char clear_bfr[size];
	memset(clear_bfr, 0 ,size);

	FILE *fp = fopen(DB_PATH,"r+");
	fseek(fp,file->txt_start, SEEK_SET);
	size_t written = fwrite(clear_bfr,1,size-1,fp);//size - 1, if not deletes ^\
	fclose(fp);

	//Set Meta To defaults.
	strcpy(file->name,"No-Name");
	strcpy(file->user,"No-User");
	file->state = FILE_CLOSED;
	file->txt_start = file->file_start + BUFFER_SIZE;
	file->file_ptr = files->txt_start + strlen(TEXT_START);
	file->block_info = EMPTY;
	SaveFileToDB(*file);

	UpdateDeleteResult(&result,"Delete Success\n");
    return &result;
}

close_output *close_file_1_svc(close_input *argp, struct svc_req *rqstp) {
    static close_output result;
	fileMeta closing_file = files[argp->fd];
	closing_file.state = FILE_CLOSED;
	SaveFileToDB(closing_file);

	result.out_msg.out_msg_len=40;

	free(result.out_msg.out_msg_val);
	result.out_msg.out_msg_val=(char *) malloc(result.out_msg.out_msg_len);
	strcpy(result.out_msg.out_msg_val, "Closed file ");
	strcat(result.out_msg.out_msg_val, closing_file.name);
	strcat(result.out_msg.out_msg_val, "\n");

	printf("Closing file %s\n",closing_file.name);
    return &result;
}

seek_output *seek_position_1_svc(seek_input *argp, struct svc_req *rqstp) {
    static seek_output result;
	int fd = argp->fd;
	fileMeta* file = &files[fd];

	// FILE* fp = fopen(DB_PATH,"r");
	long offset = file->txt_start + argp->position;
	long end = file->txt_start + FILE_BLOCK - BLOCK;
	if (offset > end){
		printf("Will reach end of file. Stopping action.\n");
	}

	// printf("NEW PTR LOCATION: %ld\n", offset);
	file->file_ptr = offset;
	// fseek(fp, offset, SEEK_SET);

	// fclose(fp);
	SaveFileToDB(*file);

	char *msg = "Delete Success";
	result.out_msg.out_msg_len = strlen(msg);
	free(result.out_msg.out_msg_val);
	result.out_msg.out_msg_val = strdup(msg);
    return &result;
}
