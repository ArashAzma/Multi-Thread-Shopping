#ifndef CAT_H
#define CAT_H

#define MAX_CATEGORIES 100
#define MAX_FILES_PER_CATEGORY 100
#define MAX_FILENAME_LENGTH 256
#define MAX_NAME_LENGTH 100

typedef struct {
    char name[MAX_NAME_LENGTH];
} ProductInfo;

int extract_names_from_files_graphic(const char* base_dir, ProductInfo* names_array, int* total_names);

#endif