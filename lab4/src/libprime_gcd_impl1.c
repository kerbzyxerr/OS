#include "../include/libprime_gcd.h"

static int is_prime_naive(int n) {
    if (n < 2) return 0;
    for (int i = 2; i < n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int prime_count(int a, int b) {
    int count = 0;
    for (int i = a; i <= b; i++) {
        if (is_prime_naive(i)) {
            count++;
        }
    }
    return count;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}