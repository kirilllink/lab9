#include "service.h"
#include <string.h>

Service* linear_search(Service *services, int size, char *key) {
    for (int i = 0; i < size; i++) {
        if (strcmp(services[i].service_name, key) == 0) {
            return &services[i];
        }
    }
    return NULL;
}