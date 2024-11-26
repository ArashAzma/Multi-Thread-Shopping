#include <time.h>

#define NUM_STORES 3
#define CATEGORIES_PER_STORE 8

typedef struct {
    char name[100];
    float price;
    float score;
    int entity;
    time_t last_modified;
} Product;

typedef enum {
    ELECTRONICS,
    HOME_APPLIANCES,
    SMART_HOME,
    KITCHEN,
    PERSONAL_CARE,
    FITNESS,
    OUTDOOR,
    GARDENING
} ProductCategory;

typedef struct {
    char name[50];
    ProductCategory categories[CATEGORIES_PER_STORE];
} Store;