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
    printf("Enter the number of services to generate: ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        fprintf(stderr, "Invalid input for number of services.\n");
        exit(1);
    }

    // Generate array of services
    Service *services = generate_random_service_array(size);

    // Print generated services table
    printf("\nGenerated services table:\n");
    print_services_table(services, size);

    // Choose search algorithm
    int algo_choice;
    printf("\nChoose search algorithm:\n");
    printf("1. Linear Search\n");
    printf("2. Binary Search Tree (Binary)\n");
    printf("3. Red-Black Tree (RBTree)\n");
    printf("Your choice: ");
    if (scanf("%d", &algo_choice) != 1 || (algo_choice < 1 || algo_choice > 3)) {
        fprintf(stderr, "Invalid algorithm choice.\n");
        free(services);
        exit(1);
    }

    // Input search key (service_name)
    char key[MAX_STRING_LENGTH];
    printf("\nEnter service_name to search for: ");
    if (scanf("%s", key) != 1) {
        fprintf(stderr, "Error reading search key.\n");
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
        // Build Binary Search Tree
        for (int i = 0; i < size; i++) {
            Binary_insert(&Binary_root, &services[i]);
        }
        start = clock();
        found = Binary_search(Binary_root, key);
        end = clock();
        Binary_free(Binary_root);
    } else if (algo_choice == 3) {
        RBNode *rb_root = NULL;
        // Build Red-Black Tree
        for (int i = 0; i < size; i++) {
            rb_insert(&rb_root, &services[i]);
        }
        start = clock();
        found = rb_search(rb_root, key);
        end = clock();
        rb_free(rb_root);
    }

    elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    // Display search result in table format
    if (found) {
        printf("\nFirst matching service found:\n");
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
        printf("\nService name \"%s\" was not found.\n", key);
    }

    printf("Search time: %.8f sec.\n", elapsed);
    free(services);
}



void benchmark_mode() {
    int sizes[NUM_BENCH_SIZES] = {100, 1000, 10000, 50000, 100000, 200000, 300000};
    
    // Open CSV file for writing
    FILE *fp = fopen("benchmark.csv", "w");
    if (!fp) {
        fprintf(stderr, "Error opening benchmark.csv for writing.\n");
        exit(1);
    }
    
    // Write CSV header
    fprintf(fp, "Size,Linear (sec),Binary (sec),RBTree (sec)\n");
    
    printf("\nBenchmark: Searching services by Service Name\n");
    printf("Each size will run %d iterations of random key searches; the result is averaged.\n", NUM_ITER);
    printf("Size\tLinear (sec)\tBinary (sec)\tRBTree (sec)\n");
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
    printf("\nThe results have been saved in the benchmark.csv\n");
}

int main(void) {
    // Инициализация генератора случайных чисел
    srand((unsigned) time(NULL));
    
    int mode;
    printf("Choose the way of going:\n");
    printf("1. Interactive search\n");
    printf("2. Benchmark\n");
    printf("Your choice: ");
    if (scanf("%d", &mode) != 1) {
        fprintf(stderr, "Error input.\n");
        exit(1);
    }
    
    if (mode == 1) {
        interactive_mode();
    } else if (mode == 2) {
        benchmark_mode();
    } else {
        printf("Invalid input of choice.\n");
    }
    
    return 0;
}
