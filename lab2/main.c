#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "montecarlo.h"

#define PI 3.14159265358979323846

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <max_threads> <total_points> <radius>\n", argv[0]);
        return 1;
    }
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    uint64_t max_threads = strtoull(argv[1], NULL, 10);
    uint64_t total_points = strtoull(argv[2], NULL, 10);
    double radius = strtod(argv[3], NULL);
    
    if (max_threads == 0 || total_points == 0 || radius <= 0) {
        fprintf(stderr, "Error: invalid parameters\n");
        return 1;
    }
    
    uint64_t points_per_thread = total_points / max_threads;
    uint64_t extra_points = total_points % max_threads;
    
    pthread_t* threads = malloc(sizeof(pthread_t) * max_threads);
    ThreadData* thread_data = malloc(sizeof(ThreadData) * max_threads);
    
    if (!threads || !thread_data) {
        fprintf(stderr, "Error: memory allocation failed\n");
        free(threads);
        free(thread_data);
        return 1;
    }
    
    uint32_t base_seed = time(NULL);
    
    for (uint64_t i = 0; i < max_threads; ++i) {
        thread_data[i].radius = radius;
        thread_data[i].points_per_thread = points_per_thread + (i < extra_points ? 1 : 0);
        thread_data[i].seed = base_seed + i;
        thread_data[i].hits = 0;
    }
    
    for (uint64_t i = 0; i < max_threads; ++i) {
        if (pthread_create(&threads[i], NULL, monte_carlo_thread, &thread_data[i]) != 0) {
            fprintf(stderr, "Error creating thread %lu\n", i);
            for (uint64_t j = 0; j < i; ++j) {
                pthread_join(threads[j], NULL);
            }
            free(threads);
            free(thread_data);
            return 1;
        }
    }
    
    for (uint64_t i = 0; i < max_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    uint64_t total_hits = 0;
    for (uint64_t i = 0; i < max_threads; ++i) {
        total_hits += thread_data[i].hits;
    }
    
    double estimated_area = 4.0 * radius * radius * (double)total_hits / total_points;
    double exact_area = PI * radius * radius;
    
    printf("Radius: %.6f\n", radius);
    printf("Total points: %lu\n", total_points);
    printf("Points inside circle: %lu\n", total_hits);
    printf("Estimated area: %.6f\n", estimated_area);
    printf("Exact area: %.6f\n", exact_area);
    printf("Error: %.6f\n", estimated_area - exact_area);
    printf("Threads used: %lu\n", max_threads);
    
    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double elapsed_ms = seconds * 1000.0 + microseconds / 1000.0;
    
    printf("Execution time: %.3f ms\n", elapsed_ms);
    
    free(threads);
    free(thread_data);
    
    return 0;
}