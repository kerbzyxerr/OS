#include "../include/server.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/select.h>
#include <semaphore.h>
#include <time.h>

typedef struct {
    char data[4096];
    size_t size;
    int flag;
} SharedData;

static void int_to_str(int num, char* str, int max_len) {
    int i = 0;
    int is_negative = 0;
    
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    do {
        str[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0 && i < max_len - 1);
    
    if (is_negative && i < max_len - 1) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void run_server_process(const char* filename) {
    pid_t pid = getpid();
    
    char shm_name[256];
    char sem_name[256];
    
    const char* shm_base = "/lab3_shm_";
    const char* sem_base = "/lab3_sem_";
    
    int pos = 0;
    const char* p = shm_base;
    while (*p && pos < 255) {
        shm_name[pos++] = *p++;
    }
    
    char pid_str[32];
    int_to_str(pid, pid_str, sizeof(pid_str));
    p = pid_str;
    while (*p && pos < 255) {
        shm_name[pos++] = *p++;
    }
    shm_name[pos] = '\0';
    
    pos = 0;
    p = sem_base;
    while (*p && pos < 255) {
        sem_name[pos++] = *p++;
    }
    
    p = pid_str;
    while (*p && pos < 255) {
        sem_name[pos++] = *p++;
    }
    sem_name[pos] = '\0';
    
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        const char msg[] = "error: failed to create shared memory\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }
    
    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        const char msg[] = "error: failed to set shared memory size\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        shm_unlink(shm_name);
        exit(EXIT_FAILURE);
    }
    
    SharedData* shared_data = mmap(NULL, sizeof(SharedData), 
                                   PROT_READ | PROT_WRITE, 
                                   MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        const char msg[] = "error: failed to map shared memory\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        shm_unlink(shm_name);
        exit(EXIT_FAILURE);
    }
    
    close(shm_fd);
    
    shared_data->size = 0;
    shared_data->flag = 0;
    
    sem_t* semaphore = sem_open(sem_name, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED) {
        const char msg[] = "error: failed to create semaphore\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        munmap(shared_data, sizeof(SharedData));
        shm_unlink(shm_name);
        exit(EXIT_FAILURE);
    }
    
    pid_t client_pid = fork();
    if (client_pid == -1) {
        const char msg[] = "error: failed to fork\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        sem_close(semaphore);
        sem_unlink(sem_name);
        munmap(shared_data, sizeof(SharedData));
        shm_unlink(shm_name);
        exit(EXIT_FAILURE);
    }
    
    if (client_pid == 0) {

        char pid_arg[32];
        int_to_str(pid, pid_arg, sizeof(pid_arg));
        
        execl("./bin/client", "client", filename, shm_name, sem_name, pid_arg, NULL);
        
        const char msg[] = "error: failed to exec client\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    } else {
        char buffer[4096];
        ssize_t bytes_read;
        int stdin_eof = 0;
        
        while (!stdin_eof) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);
            
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000; 
            
            int activity = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
            
            if (activity < 0) {
                const char msg[] = "error: select failed\n";
                write(STDERR_FILENO, msg, sizeof(msg) - 1);
                break;
            }
            
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
                if (bytes_read > 0) {
                    sem_wait(semaphore);
                    memcpy(shared_data->data, buffer, bytes_read);
                    shared_data->size = bytes_read;
                    shared_data->flag = 0;
                    sem_post(semaphore);
                    
                    while (1) {
                        sem_wait(semaphore);
                        if (shared_data->flag == 1) {
                            if (shared_data->size > 0 && 
                                memcmp(shared_data->data, "Error:", 6) == 0) {
                                write(STDERR_FILENO, shared_data->data, shared_data->size);
                            }
                            sem_post(semaphore);
                            break;
                        }
                        sem_post(semaphore);
                        
                        struct timespec ts;
                        ts.tv_sec = 0;
                        ts.tv_nsec = 10000000; 
                        nanosleep(&ts, NULL);
                    }
                } else if (bytes_read == 0) {
                    stdin_eof = 1;
                    sem_wait(semaphore);
                    shared_data->flag = 2;
                    sem_post(semaphore);
                }
            }
        }
        
        int status;
        waitpid(client_pid, &status, 0);
        
        sem_close(semaphore);
        sem_unlink(sem_name);
        munmap(shared_data, sizeof(SharedData));
        shm_unlink(shm_name);
    }
}