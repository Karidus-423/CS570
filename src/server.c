#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "ssnfs.h"

typedef char block[512]; // 1 Block = 512bytes
typedef struct file_meta{
	char user[20];
	char name[20];
	int int_desc;
	block size;
	block used;
}file_meta;

struct stat st ={0};

const char *db_dir = "./src/database/";

void SetupDB(){
	int check_dr = stat(db_dir,&st);
	if(check_dr != 0){
		int mk_dir = mkdir(db_dir, 0700);
		if (mk_dir != 0){
			fprintf(stderr, "ERROR: %s\n",strerror(errno));
			exit(0);
		}
		FILE *mk_table = fopen("file.tb","w");
		FILE *mk_db = fopen("database.db","w");
		fclose(mk_table);
		fclose(mk_db);
	}
}

open_output *open_file_1_svc(open_input *argp, struct svc_req *rqstp) {
    static open_output result;
	SetupDB();
    result.fd = 20;
    result.out_msg.out_msg_len = 10;
    free(result.out_msg.out_msg_val);
    result.out_msg.out_msg_val = (char *)malloc(result.out_msg.out_msg_len + 1);
    strcpy(result.out_msg.out_msg_val, (*argp).file_name);
    printf("In server: filename recieved:%s\n", argp->file_name);
    printf("In server username received:%s\n", argp->user_name);
    return &result;
}

read_output *read_file_1_svc(read_input *argp, struct svc_req *rqstp) {
    static read_output result;

    return &result;
}

write_output *write_file_1_svc(write_input *argp, struct svc_req *rqstp) {
    static write_output result;

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
