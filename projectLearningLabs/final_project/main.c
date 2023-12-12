/**
 * \author Kobe Michiels
 */

#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"

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
    // create new, empty gateway.log file
    fclose(fopen("gateway.log", "w"));
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
            }
            else {
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
    // create log process
    int pipe_fd = create_log_process();

    // initialize buffer
    sbuffer_init(&sbuffer);

    // create connection, data and storage manager threads
    pthread_t connection_manager_thread, data_manager_thread, storage_manager_thread;

    conn_thread_arg_t *conn_thread_arg = malloc(sizeof(conn_thread_arg_t*));
    conn_thread_arg->port = atoi(argv[1]);
    conn_thread_arg->max_conn = atoi(argv[2]);
    conn_thread_arg->buffer = sbuffer;
    conn_thread_arg->fd = pipe_fd;

    data_thread_arg_t *data_thread_arg = malloc(sizeof(data_thread_arg_t*));
    data_thread_arg->buffer = sbuffer;
    data_thread_arg->fd = pipe_fd;

    stor_thread_arg_t *stor_thread_arg = malloc(sizeof(stor_thread_arg_t*));
    stor_thread_arg->buffer = sbuffer;
    stor_thread_arg->fd = pipe_fd;

    pthread_create(&connection_manager_thread, NULL, connection_manager, conn_thread_arg);
    pthread_create(&data_manager_thread, NULL, data_manager, data_thread_arg);
    pthread_create(&storage_manager_thread, NULL, storage_manager, stor_thread_arg);

    pthread_join(connection_manager_thread, NULL);
    pthread_join(data_manager_thread, NULL);
    pthread_join(storage_manager_thread, NULL);

    // end log process when threads are terminated
    char write_msg[SIZE];
    sprintf(write_msg, "%s", "Terminate process");
    write(fd[WRITE_END], write_msg, SIZE);

    // free allocated memory resources
    free(conn_thread_arg);
    free(data_thread_arg);
    free(stor_thread_arg);
    sbuffer_free(&sbuffer);
    return 0;
}