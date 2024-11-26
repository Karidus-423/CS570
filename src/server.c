#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gen/ssnfs.h"
#include "serve.h"

//Should only update once per lifetime. As all clients send "01CheckStep".
int lifetime_checks = 0;

open_output *open_file_1_svc(open_input *argp, struct svc_req *rqstp) {
    static open_output result;
	//Check if database is present. 
	if ((strcmp(argp->file_name, "01CheckStep")== 0) && lifetime_checks == 0){
		printf("Check Step Recieved.\n");
		SetupDB();
		return &result;
	}else{
		result.fd = FindFile(argp->user_name, argp->file_name);
		if (result.fd == -1){
			//File not found.
			if (AddFile(argp->user_name, argp->file_name) == 0){
				result.fd = FindFile(argp->user_name, argp->file_name);
			}else{
				printf("Unable to add file");
				exit(EXIT_FAILURE);
			}
		}else{
			result.out_msg.out_msg_len=10;
			free(result.out_msg.out_msg_val);
			result.out_msg.out_msg_val=(char *) malloc(result.out_msg.out_msg_len);
			strcpy(result.out_msg.out_msg_val, (*argp).file_name);
			printf("In server: filename recieved:%s\n",argp->file_name);
			printf("In server username received:%s\n",argp->user_name);
		}
		return &result;
	}
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
