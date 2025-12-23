#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/libprime_gcd.h"

int main() {
    char input[256];
    int command;
    int lib_loaded = 1; 

    printf("Программа со статической линковкой\n");
    printf("Используется реализация %d\n", lib_loaded);
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
            printf("В статически слинкованной программе реализация фиксирована на этапе компиляции.\n");
            printf("Текущая реализация: %d\n", lib_loaded);
            continue;
        }
        
        int a, b;
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
    
    return 0;
}