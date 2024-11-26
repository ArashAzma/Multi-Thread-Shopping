#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

Store stores[NUM_STORES];



const char* category_names[] = {
    "Electronics", "Home Appliances", "Smart Home", "Kitchen",
    "Personal Care", "Fitness", "Outdoor", "Gardening"
};

void generate_dummy_data() {
    srand(time(NULL));

    const char* store_names[NUM_STORES] = {"Digikala", "MashhadKala", "Torob"};
    const char* product_names[] = {"Smart Showerhead", "Robot Vacuum", "Blender", "Hair Dryer",
                                    "Fitness Tracker", "BBQ Grill", "Garden Hose", "Wireless Speaker"};
    const int num_products = sizeof(product_names) / sizeof(product_names[0]);

    for (int i = 0; i < NUM_STORES; i++) {
        strcpy(stores[i].name, store_names[i]);
        for (int j = 0; j < CATEGORIES_PER_STORE; j++) {
            stores[i].categories[j].category = (ProductCategory)j;
            stores[i].categories[j].product_count = rand() % 10;

            for (int k = 0; k < stores[i].categories[j].product_count; k++) {
                Product* product = &stores[i].categories[j].products[k];
                strcpy(product->name, product_names[rand() % num_products]);
                product->price = (float)(rand() % 1000) / 10.0f;
                product->score = (float)(rand() % 50) / 10.0f;
                product->entity = rand() % 50;
                product->last_modified = time(NULL) - rand() % (60 * 60 * 24 * 30); // Random date within last month
            }
        }
    }
}
void save_data_to_binary_file(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    fwrite(stores, sizeof(Store), NUM_STORES, file);

    fclose(file);
    printf("Data saved to %s\n", filename);
}

void load_data_from_binary_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return;
    fread(stores, sizeof(Store), NUM_STORES, file);

    fclose(file);
}

void print_stores() {
    for (int i = 0; i < NUM_STORES; i++) {
        printf("Store: %s\n", stores[i].name);
        for (int j = 0; j < CATEGORIES_PER_STORE; j++) {
            printf("  Category: %s\n", category_names[stores[i].categories[j].category]);
            for (int k = 0; k < stores[i].categories[j].product_count; k++) {
                Product* product = &stores[i].categories[j].products[k];
                printf("    Product: %s, Price: %.2f, Score: %.1f, Entity: %d, Last Modified: %s",
                       product->name, product->price, product->score, product->entity,
                       ctime(&product->last_modified));
            }
        }
    }
}

int main() {
    const char* filename = "dataset.bin";

    generate_dummy_data();
    save_data_to_binary_file(filename);

    memset(stores, 0, sizeof(stores));

    load_data_from_binary_file(filename);
    print_stores();

    return 0;
}