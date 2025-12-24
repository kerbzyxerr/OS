#include "./include/parent.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        char filename[256];
        const char prompt[] = "Enter filename: ";
        write(STDOUT_FILENO, prompt, sizeof(prompt) - 1);
        
        ssize_t bytes_read = read(STDIN_FILENO, filename, sizeof(filename) - 1);
        if (bytes_read <= 0) {
            const char msg[] = "error: failed to read filename\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            exit(EXIT_FAILURE);
        }
        
        if (bytes_read > 0 && filename[bytes_read - 1] == '\n') {
            filename[bytes_read - 1] = '\0';
        } else {
            filename[bytes_read] = '\0';
        }
        
        run_parent_process(filename);
    } else {
        run_parent_process(argv[1]);
    }
    return 0;
}