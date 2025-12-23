#include "../include/parent.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

void run_parent_process(const char* filename) {
    int pipe1[2], pipe2[2];
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        const char msg[] = "error: failed to create pipes\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        const char msg[] = "error: failed to fork\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe1[1]);  
        close(pipe2[0]);  

        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);

        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);

        execl("./bin/child", "child", filename, NULL);
        
        const char msg[] = "error: failed to exec child\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    } else {
        close(pipe1[0]);  
        close(pipe2[1]);  

        char buffer[4096];
        ssize_t bytes_read;
        
        while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
            ssize_t written = write(pipe1[1], buffer, bytes_read);
            if (written == -1) {
                const char msg[] = "error: failed to write to pipe\n";
                write(STDERR_FILENO, msg, sizeof(msg) - 1);
                break;
            }
            
            char response[4096];
            ssize_t nread = read(pipe2[0], response, sizeof(response) - 1);
            if (nread > 0) {
                response[nread] = '\0';
                
                if (strstr(response, "Error:") == response) {
                    write(STDOUT_FILENO, response, nread);
                }
            }
        }

        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);
    }
}