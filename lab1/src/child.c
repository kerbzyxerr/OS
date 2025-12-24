#include "../include/child.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int check_rule(const char* str) {
    size_t len = strlen(str);
    if (len == 0) return 0;
    
    for (size_t i = len - 1; i > 0; --i) {
        if (str[i] != '\n' && str[i] != '\r' && str[i] != ' ' && str[i] != '\t') {
            return (str[i] == '.' || str[i] == ';');
        }
    }
    
    return (str[0] == '.' || str[0] == ';');
}

void run_child_process(const char* filename) {
    int file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file == -1) {
        const char error_msg[] = "Error: failed to open file\n";
        write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
        exit(EXIT_FAILURE);
    }

    char buffer[4096];
    ssize_t bytes_read;
    
    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        
        char* line = buffer;
        char* next_line;
        
        while ((next_line = strchr(line, '\n')) != NULL) {
            *next_line = '\0';  

            if (check_rule(line)) {
                size_t line_len = strlen(line);
                write(file, line, line_len);
                write(file, "\n", 1);
            } else {
                const char error_msg[] = "Error: String does not end with '.' or ';'\n";
                write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
            }
            
            line = next_line + 1;
        }
        
        if (strlen(line) > 0) {
            if (check_rule(line)) {
                size_t line_len = strlen(line);
                write(file, line, line_len);
                write(file, "\n", 1);
            } else {
                const char error_msg[] = "Error: String does not end with '.' or ';'\n";
                write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
            }
        }
    }
    
    close(file);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        const char error_msg[] = "Error: missing filename argument\n";
        write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
        return EXIT_FAILURE;
    }
    
    run_child_process(argv[1]);
    return 0;
}