#ifndef SERVICE_H
#define SERVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_STRING_LENGTH 4

typedef struct t {
    char service_name[MAX_STRING_LENGTH];
    char service_type[MAX_STRING_LENGTH];
    int cost;
    int duration;
    char master_name[MAX_STRING_LENGTH];
} Service;

#include "service.h"
#include <string.h>

// Функция генерации случайного целого числа от a до b.
int generate_random_int(int a, int b) {
    return random() % (b - a) + a;
}

// Генерация случайного символа.
char generate_random_char() {
    return (char) generate_random_int(65, 90);
}

// Генерация случайной строки. Длина выбирается случайно от 1 до MAX_STRING_LENGTH - 1.
void generate_random_str(char *dst) {
    int len = generate_random_int(1, MAX_STRING_LENGTH);
    for (int i = 0; i < len; i++) {
        dst[i] = generate_random_char();
    }
    dst[len] = '\0';
}

// Генерация случайного сервиса. Здесь генерируется ключевое поле service_name и остальные поля.
void generate_random_service(Service *service) {
    generate_random_str(service->service_name);
    generate_random_str(service->service_type);
    service->cost = generate_random_int(1, 10000);
    service->duration = generate_random_int(1, 10000);
    generate_random_str(service->master_name);
}

// Генерация массива случайных сервисов.
Service * generate_random_service_array(int size) {
    Service *services = (Service *)malloc(size * sizeof(Service));
    if (!services) {
        fprintf(stderr, "Ошибка выделения памяти для сервиса\n");
        exit(1);
    }
    for (int i = 0; i < size; i++) {
        generate_random_service(&services[i]);
    }
    return services;
}

#endif