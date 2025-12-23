#pragma once

#include <stdint.h>
#include <pthread.h>

typedef struct {
    uint64_t points_per_thread;
    double radius;
    uint32_t seed;
    uint64_t hits;
} ThreadData;

void* monte_carlo_thread(void* arg);