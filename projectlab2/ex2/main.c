#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#define SIZE 25
#define READ_END 0
#define WRITE_END 1

int main() {

    char parent_string[SIZE] = "Hi There";
    char child_string[SIZE];
    pid_t pid;
    int fd[2];
// create the pipe
    if (pipe(fd) == -1){
        printf("Pipe failed\n");
        return 1;
    }
// fork the child
    pid = fork();
    if (pid < 0){ // fork error
        printf("fork failed\n");
        return 1;
    }
    if (pid > 0){ // parent process
        close(fd[READ_END]); // parent closes the reading end of the pipe
        write(fd[WRITE_END], parent_string, strlen(parent_string)+1); // parent writes parent_string to the pipe
        close(fd[WRITE_END]); // parent closes the writing end of the pipe
        wait(NULL); // parent waits for the child
    }
    else{ // child process
        close(fd[WRITE_END]); // child closes the writing end of the pipe
        read(fd[READ_END], child_string, SIZE); // child reads from the pipe into child_string
        for (int i = 0; i < strlen(child_string); i++){ // character case conversion
            if (islower(child_string[i])) {
                child_string[i] = (char)toupper(child_string[i]);
            }
            else {
                child_string[i] = (char)tolower(child_string[i]);
            }
            printf("%c", child_string[i]);
        }
        printf("\n");
        close(fd[READ_END]); // child closes reading end of the pipe
    }
    return 0;
}

