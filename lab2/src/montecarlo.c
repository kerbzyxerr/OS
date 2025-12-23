#include "../include/montecarlo.h"
#include <stdint.h>

static uint32_t lcg(uint32_t* seed) {
    *seed = *seed * 1103515245 + 12345;
    return *seed;
}

void* monte_carlo_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->hits = 0;
    
    uint32_t seed = data->seed;
    
    for (uint64_t i = 0; i < data->points_per_thread; ++i) {
        double x = (double)lcg(&seed) / (1ULL << 32) * 2 * data->radius - data->radius;
        double y = (double)lcg(&seed) / (1ULL << 32) * 2 * data->radius - data->radius;
        
        if (x*x + y*y <= data->radius * data->radius) {
            data->hits++;
        }
    }
    
    return NULL;
}