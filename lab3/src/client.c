#include "../include/client.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

typedef struct {
    char data[4096];
    size_t size;
    int flag;
} SharedData;

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

void run_client_process(const char* filename, const char* shm_name, const char* sem_name) {
    int file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file == -1) {
        const char error_msg[] = "Error: failed to open file\n";
        write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
        exit(EXIT_FAILURE);
    }
    
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        const char error_msg[] = "Error: failed to open shared memory\n";
        write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
        close(file);
        exit(EXIT_FAILURE);
    }
    
    SharedData* shared_data = mmap(NULL, sizeof(SharedData), 
                                   PROT_READ | PROT_WRITE, 
                                   MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        const char error_msg[] = "Error: failed to map shared memory\n";
        write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
        close(shm_fd);
        close(file);
        exit(EXIT_FAILURE);
    }
    
    close(shm_fd);
    
    sem_t* semaphore = sem_open(sem_name, 0);
    if (semaphore == SEM_FAILED) {
        const char error_msg[] = "Error: failed to open semaphore\n";
        write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
        munmap(shared_data, sizeof(SharedData));
        close(file);
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        sem_wait(semaphore);
        
        if (shared_data->flag == 2) {
            sem_post(semaphore);
            break;
        }
        
        if (shared_data->size > 0 && shared_data->flag == 0) {
            char buffer[4096];
            memcpy(buffer, shared_data->data, shared_data->size);
            buffer[shared_data->size] = '\0';
            
            char* line = buffer;
            char* next_line;
            
            while ((next_line = strchr(line, '\n')) != NULL) {
                *next_line = '\0';
                
                if (check_rule(line)) {
                    size_t line_len = strlen(line);
                    write(file, line, line_len);
                    write(file, "\n", 1);
                    shared_data->flag = 1;
                } else {
                    const char error_msg[] = "Error: String does not end with '.' or ';'\n";
                    memcpy(shared_data->data, error_msg, sizeof(error_msg) - 1);
                    shared_data->size = sizeof(error_msg) - 1;
                    shared_data->flag = 1;
                }
                
                line = next_line + 1;
            }
            
            if (strlen(line) > 0) {
                if (check_rule(line)) {
                    size_t line_len = strlen(line);
                    write(file, line, line_len);
                    write(file, "\n", 1);
                    shared_data->flag = 1;
                } else {
                    const char error_msg[] = "Error: String does not end with '.' or ';'\n";
                    memcpy(shared_data->data, error_msg, sizeof(error_msg) - 1);
                    shared_data->size = sizeof(error_msg) - 1;
                    shared_data->flag = 1;
                }
            }
        }
        
        sem_post(semaphore);
        
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 10000000; 
        nanosleep(&ts, NULL);
    }
    
    sem_close(semaphore);
    munmap(shared_data, sizeof(SharedData));
    close(file);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        const char error_msg[] = "Error: missing arguments\n";
        write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
        return EXIT_FAILURE;
    }
    
    run_client_process(argv[1], argv[2], argv[3]);
    return 0;
}