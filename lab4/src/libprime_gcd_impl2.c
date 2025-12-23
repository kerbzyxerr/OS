#include "../include/libprime_gcd.h"
#include <stdlib.h>
#include <string.h>

int prime_count(int a, int b) {
    if (b < 2 || a > b) return 0;
    if (a < 2) a = 2;
    
    int size = b - a + 1;
    char* is_prime = (char*)malloc(size);
    if (!is_prime) return -1;
    
    memset(is_prime, 1, size);
    
    for (int p = 2; p * p <= b; p++) {
        int start = ((a + p - 1) / p) * p;
        if (start < p * p) start = p * p;
        
        for (int i = start; i <= b; i += p) {
            if (i >= a) {
                is_prime[i - a] = 0;
            }
        }
    }
    
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (is_prime[i]) {
            count++;
        }
    }
    
    free(is_prime);
    return count;
}

int gcd(int a, int b) {
    int min = a < b ? a : b;
    for (int i = min; i >= 1; i--) {
        if (a % i == 0 && b % i == 0) {
            return i;
        }
    }
    return 1;
}