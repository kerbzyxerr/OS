#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

typedef int (*prime_count_func)(int, int);
typedef int (*gcd_func)(int, int);

int main() {
    void* lib_handle = NULL;
    prime_count_func prime_count = NULL;
    gcd_func gcd = NULL;
    
    int current_lib = 1; 
    char input[256];
    
    lib_handle = dlopen("./libprime_gcd_impl1.so", RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки 1: %s\n", dlerror());
        return 1;
    }
    
    prime_count = (prime_count_func)dlsym(lib_handle, "prime_count");
    gcd = (gcd_func)dlsym(lib_handle, "gcd");
    
    if (!prime_count || !gcd) {
        fprintf(stderr, "Ошибка загрузки функций: %s\n", dlerror());
        dlclose(lib_handle);
        return 1;
    }
    
    printf("Программа с динамической загрузкой библиотек\n");
    printf("Текущая реализация: %d\n", current_lib);
    printf("Команды:\n");
    printf("  0 - переключить реализацию\n");
    printf("  1 a b - подсчёт простых чисел на отрезке [a, b]\n");
    printf("  2 a b - НОД(a, b)\n");
    printf("  q - выход\n");
    
    while (1) {
        printf("\n> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        
        if (input[0] == 'q') break;
        
        if (input[0] == '0') {
            dlclose(lib_handle);
            
            if (current_lib == 1) {
                lib_handle = dlopen("./libprime_gcd_impl2.so", RTLD_LAZY);
                current_lib = 2;
            } else {
                lib_handle = dlopen("./libprime_gcd_impl1.so", RTLD_LAZY);
                current_lib = 1;
            }
            
            if (!lib_handle) {
                fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
                return 1;
            }
            
            prime_count = (prime_count_func)dlsym(lib_handle, "prime_count");
            gcd = (gcd_func)dlsym(lib_handle, "gcd");
            
            if (!prime_count || !gcd) {
                fprintf(stderr, "Ошибка загрузки функций: %s\n", dlerror());
                dlclose(lib_handle);
                return 1;
            }
            
            printf("Переключено на реализацию %d\n", current_lib);
            continue;
        }
        
        int command, a, b;
        if (sscanf(input, "%d %d %d", &command, &a, &b) == 3) {
            switch (command) {
                case 1: {
                    int result = prime_count(a, b);
                    if (result >= 0) {
                        printf("Количество простых чисел на отрезке [%d, %d]: %d\n", a, b, result);
                    } else {
                        printf("Ошибка вычисления\n");
                    }
                    break;
                }
                case 2: {
                    int result = gcd(a, b);
                    printf("НОД(%d, %d) = %d\n", a, b, result);
                    break;
                }
                default:
                    printf("Неизвестная команда\n");
            }
        } else {
            printf("Неверный формат команды\n");
        }
    }
    
    if (lib_handle) {
        dlclose(lib_handle);
    }
    
    return 0;
}