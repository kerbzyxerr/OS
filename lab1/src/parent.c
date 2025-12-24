#include "../include/parent.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>

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

        int flags = fcntl(pipe2[0], F_GETFL, 0);
        fcntl(pipe2[0], F_SETFL, flags | O_NONBLOCK);
        
        char buffer[4096];
        ssize_t bytes_read;
        int stdin_eof = 0;
        
        while (!stdin_eof) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);
            FD_SET(pipe2[0], &readfds);
            
            int max_fd = (STDIN_FILENO > pipe2[0]) ? STDIN_FILENO : pipe2[0];
            
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000;
            
            int activity = select(max_fd + 1, &readfds, NULL, NULL, &timeout);
            
            if (activity < 0) {
                const char msg[] = "error: select failed\n";
                write(STDERR_FILENO, msg, sizeof(msg) - 1);
                break;
            }
            
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
                if (bytes_read > 0) {
                    write(pipe1[1], buffer, bytes_read);
                } else if (bytes_read == 0) {
                    stdin_eof = 1;
                    close(pipe1[1]);
                }
            }
            
            if (FD_ISSET(pipe2[0], &readfds)) {
                bytes_read = read(pipe2[0], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    write(STDOUT_FILENO, buffer, bytes_read);
                } else if (bytes_read == 0) {
                    break;
                }
            }
        }
        
        close(pipe2[0]);
        
        int status;
        waitpid(pid, &status, 0);
    }
}