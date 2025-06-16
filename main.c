#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "service.h"
#include "linear_search.h"
#include "binary_search.h"
#include "rbtree.h"

#define NUM_BENCH_SIZES 7
#define NUM_ITER 300000  // количество случайных ключей для замера в каждом тесте

// Функция для вывода таблицы сервисов (интерактивный режим)
void print_services_table(Service *services, int size) {
    printf("--------------------------------------------------------------------------------\n");
    printf("| %-15s | %-15s | %-7s | %-8s | %-15s |\n", 
           "Service Name", "Service Type", "Cost", "Duration", "Master Name");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < size; i++) {
        printf("| %-15s | %-15s | %-7d | %-8d | %-15s |\n",
               services[i].service_name,
               services[i].service_type,
               services[i].cost,
               services[i].duration,
               services[i].master_name);
    }
    printf("--------------------------------------------------------------------------------\n");
}

// Режим интерактивного поиска: выбирается алгоритм, генерируется таблица, затем выполняется поиск с засечением времени.
void interactive_mode() {
    int size;
    printf("Введите количество сервисов для генерации: ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        fprintf(stderr, "Ошибка ввода количества сервисов.\n");
        exit(1);
    }
    
    // Генерация массива сервисов
    Service *services = generate_random_service_array(size);
    
    // Вывод сгенерированной таблицы сервисов
    printf("\nСгенерированная таблица сервисов:\n");
    print_services_table(services, size);
    
    // Выбор алгоритма поиска
    int algo_choice;
    printf("\nВыберите алгоритм поиска:\n");
    printf("1. Линейный поиск\n");
    printf("2. Двоичное дерево поиска (Binary)\n");
    printf("3. Красно-чёрное дерево (RBTree)\n");
    printf("Ваш выбор: ");
    if (scanf("%d", &algo_choice) != 1 || (algo_choice < 1 || algo_choice > 3)) {
        fprintf(stderr, "Неверный выбор алгоритма.\n");
        free(services);
        exit(1);
    }
    
    // Запрос ключа (service_name) для поиска
    char key[MAX_STRING_LENGTH];
    printf("\nВведите service_name для поиска: ");
    if (scanf("%s", key) != 1) {
        fprintf(stderr, "Ошибка ввода ключа поиска.\n");
        free(services);
        exit(1);
    }
    
    clock_t start, end;
    double elapsed;
    Service *found = NULL;
    
    if (algo_choice == 1) {
        start = clock();
        found = linear_search(services, size, key);
        end = clock();
    } else if (algo_choice == 2) {
        BinaryNode *Binary_root = NULL;
        // Построение дерева
        for (int i = 0; i < size; i++) {
            Binary_insert(&Binary_root, &services[i]);
        }
        start = clock();
        found = Binary_search(Binary_root, key);
        end = clock();
        Binary_free(Binary_root);
    } else if (algo_choice == 3) {
        RBNode *rb_root = NULL;
        // Построение дерева
        for (int i = 0; i < size; i++) {
            rb_insert(&rb_root, &services[i]);
        }
        start = clock();
        found = rb_search(rb_root, key);
        end = clock();
        rb_free(rb_root);
    }
    
    elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    // Вывод результата поиска в виде таблицы
    if (found) {
        printf("\nНайденное первое вхождение сервиса:\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("| %-15s | %-15s | %-7s | %-8s | %-15s |\n",
               "Service Name", "Service Type", "Cost", "Duration", "Master Name");
        printf("--------------------------------------------------------------------------------\n");
        printf("| %-15s | %-15s | %-7d | %-8d | %-15s |\n",
               found->service_name,
               found->service_type,
               found->cost,
               found->duration,
               found->master_name);
        printf("--------------------------------------------------------------------------------\n");
    } else {
        printf("\nСервис с названием \"%s\" не найден.\n", key);
    }
    
    printf("Время поиска: %.8f сек.\n", elapsed);
    free(services);
}


void benchmark_mode() {
    int sizes[NUM_BENCH_SIZES] = {100, 1000, 10000, 50000, 100000, 200000, 300000};
    
    // Открываем CSV-файл для записи
    FILE *fp = fopen("benchmark.csv", "w");
    if (!fp) {
        fprintf(stderr, "Ошибка открытия файла benchmark.csv для записи.\n");
        exit(1);
    }
    
    // Записываем заголовок CSV
    fprintf(fp, "Размер,Линейный (сек),Binary (сек),RBTree (сек)\n");
    
    printf("\nБенчмарк поиска сервисов по Service Name\n");
    printf("Для каждого размера выполняется %d итераций поиска случайных ключей, результат усредняется.\n", NUM_ITER);
    printf("Размер\tЛинейный (сек)\tBinary (сек)\tRBTree (сек)\n");
    printf("----------------------------------------------------------------\n");
    
    for (int i = 0; i < NUM_BENCH_SIZES; i++) {
        int size = sizes[i];
        Service *services = generate_random_service_array(size);
        double totalLinear = 0.0, totalBinary = 0.0, totalRB = 0.0;
        Service *found = NULL;
        
        // Для Binary и RBTree создаём структуры один раз для данного набора
        BinaryNode *Binary_root = NULL;
        RBNode *rb_root = NULL;
        for (int j = 0; j < size; j++) {
            Binary_insert(&Binary_root, &services[j]);
            rb_insert(&rb_root, &services[j]);
        }
        
        // Производим NUM_ITER итераций поиска на случайно выбранных ключах 
        for (int j = 0; j < NUM_ITER; j++) {
            int randIndex = generate_random_int(0, size); // выбираем случайный индекс из [0, size)
            char key[MAX_STRING_LENGTH];
            strncpy(key, services[randIndex].service_name, MAX_STRING_LENGTH);
            key[MAX_STRING_LENGTH - 1] = '\0';
            
            clock_t start, end;
            // Линейный поиск
            start = clock();
            found = linear_search(services, size, key);
            end = clock();
            totalLinear += (double)(end - start) / CLOCKS_PER_SEC;
            
            // Поиск с Binary
            start = clock();
            found = Binary_search(Binary_root, key);
            end = clock();
            totalBinary += (double)(end - start) / CLOCKS_PER_SEC;
            
            // Поиск с RBTree
            start = clock();
            found = rb_search(rb_root, key);
            end = clock();
            totalRB += (double)(end - start) / CLOCKS_PER_SEC;
        }
        
        // Усредняем время поиска
        double avgLinear = totalLinear / NUM_ITER;
        double avgBinary = totalBinary / NUM_ITER;
        double avgRB  = totalRB / NUM_ITER;
        
        // Запись строки в CSV
        fprintf(fp, "%d,%.8f,%.8f,%.8f\n", size, avgLinear, avgBinary, avgRB);
        printf("%d\t%.8f\t%.8f\t%.8f\n", size, avgLinear, avgBinary, avgRB);
        
        // Освобождаем выделенную память
        Binary_free(Binary_root);
        rb_free(rb_root);
        free(services);
    }
    
    fclose(fp);
    printf("\nРезультаты бенчмарка сохранены в файл benchmark.csv\n");
}

int main(void) {
    // Инициализация генератора случайных чисел
    srandom((unsigned) time(NULL));
    
    int mode;
    printf("Выберите режим работы:\n");
    printf("1. Интерактивный поиск\n");
    printf("2. Бенчмарк\n");
    printf("Ваш выбор: ");
    if (scanf("%d", &mode) != 1) {
        fprintf(stderr, "Ошибка ввода режима.\n");
        exit(1);
    }
    
    if (mode == 1) {
        interactive_mode();
    } else if (mode == 2) {
        benchmark_mode();
    } else {
        printf("Неверный режим.\n");
    }
    
    return 0;
}
