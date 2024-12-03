#ifndef STORE_H
#define STORE_H

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
    ProductCategory category;
    Product products[10];
    int product_count; 
} Category;

typedef struct {
    char name[50];
    Category categories[CATEGORIES_PER_STORE];
} Store;

extern Store stores[NUM_STORES];

void generate_dummy_data();
void save_data_to_binary_file(const char* filename);
void load_data_from_binary_file(const char* filename);
void print_stores();

#endif