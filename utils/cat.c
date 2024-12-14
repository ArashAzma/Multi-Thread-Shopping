#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "../headers/cat.h"

int extract_names_from_files_graphic(const char* base_dir, ProductInfo* names_array, int* total_names) {
    DIR* store_dir;
    struct dirent* category_entry;
    char category_path[MAX_FILENAME_LENGTH];
    
    store_dir = opendir(base_dir);
    if (store_dir == NULL) {
        perror("Error opening directory");
        return 0;
    }
    
    *total_names = 0;

    while ((category_entry = readdir(store_dir)) != NULL) {
        if (strcmp(category_entry->d_name, ".") != 0 && 
            strcmp(category_entry->d_name, "..") != 0) {
            
            snprintf(category_path, sizeof(category_path), "%s/%s", base_dir, category_entry->d_name);
            
            DIR* category_dir = opendir(category_path);
            if (category_dir == NULL) {
                perror("Error opening category directory");
                continue;
            }
            
            struct dirent* file_entry;
            while ((file_entry = readdir(category_dir)) != NULL) {
                if (1) {
                    char file_path[MAX_FILENAME_LENGTH];
                    snprintf(file_path, sizeof(file_path), "%s/%s", category_path, file_entry->d_name);
                    
                    FILE* file = fopen(file_path, "r");
                    if (file == NULL) {
                        perror("Error opening file");
                        continue;
                    }
                    
                    char line[256];
                    while (fgets(line, sizeof(line), file)) {
                        if (strncmp(line, "Name:", 5) == 0) {
                            char* name_start = line + 5;
                            while (*name_start == ' ') name_start++;
                            
                            char* newline = strchr(name_start, '\n');
                            if (newline) *newline = '\0';
                            
                            strncpy(names_array[*total_names].name, name_start, MAX_NAME_LENGTH - 1);
                            names_array[*total_names].name[MAX_NAME_LENGTH - 1] = '\0';
                            
                            (*total_names)++;
                            
                            break;
                        }
                    }
                    
                    fclose(file);
                }
            }
            
            closedir(category_dir);
        }
    }
    
    closedir(store_dir);
    
    return 1;
}
