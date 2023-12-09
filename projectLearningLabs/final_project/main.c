/**
 * \author Kobe Michiels
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "config.h"
#include "connmgr.h"
#include "datamgr.h"
#define SIZE 128
#define READ_END 0
#define WRITE_END 1

pid_t pid;
int fd[2];
int sequence_number = 0;
sbuffer_t *sbuffer;

int write_to_log_process(char *msg) {
    // compose log message in format <sequence number> - <timestamp> - <log-event info message>
    char log_msg[SIZE];
    time_t t = time(NULL);
    char *timestamp = ctime(&t);
    timestamp[strlen(timestamp)-1] = '\0';
    sprintf(log_msg, "%d - %s - %s.\n",sequence_number, timestamp, msg);
    sequence_number++;

    // log message in log file
    FILE *log_file = fopen("gateway.log", "a");
    fprintf(log_file, "%s",log_msg);
    fclose(log_file);
    return 0;
}

int end_log_process() {
    close(fd[READ_END]);
    return 0;
}

int create_log_process() {
    // create the pipe
    if (pipe(fd) == -1) {
        write_to_log_process("Error: Pipe failed");
        exit(EXIT_FAILURE);
    }
    // fork the child
    pid = fork();
    if (pid < 0) { // fork error
        write_to_log_process("Error: Fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) { // parent process
        close(fd[READ_END]); // parent closes the reading end of the pipe
    } else { // child process
        char child_msg[SIZE];
        close(fd[WRITE_END]); // child closes the writing end of the pipe
        while (read(fd[READ_END], child_msg, SIZE) >= 0) {
            if (strcmp(child_msg, "Terminate process") == 0) {
                end_log_process();
                exit(EXIT_SUCCESS);
            } else {
                write_to_log_process(child_msg);
            }
        }
    }
    return fd[WRITE_END];
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Please provide the right arguments: first the port, then the max nb of clients");
        return -1;
    }

    create_log_process();

    pthread_t connmgr_thread;
    pthread_t datamgr_thread;
    //pthread_t stormgr_thread;

    conn_param_t *connmgr_param = malloc(sizeof(conn_param_t));
    connmgr_param->port = atoi(argv[1]);
    connmgr_param->max_conn = atoi(argv[2]);
    connmgr_param->buffer = sbuffer;

    data_param_t *datamgr_param = malloc(sizeof(data_param_t));
    datamgr_param->buffer = sbuffer;

    pthread_create(&connmgr_thread, NULL, connection_manager, connmgr_param);
    pthread_create(&datamgr_thread, NULL, datamgr_parse_sensor_files, datamgr_param);
    //pthread_create(&stormgr_thread, NULL, NULL, NULL);

    pthread_join(connmgr_thread, NULL);
    pthread_join(datamgr_thread, NULL);
    //pthread_join(stormgr_thread, NULL);
}

