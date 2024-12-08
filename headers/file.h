#ifndef FILE_H
#define FILE_H

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256

void get_category_name(char path[], char category_name[]);

void get_store_name(char path[], char store_name[]);

int find_item_dirs(char category_path[], char sub_dirs[MAX_SUB_DIRS][MAX_PATH_LEN]);

int find_sub_dirs(char store_path[], char sub_dirs[MAX_SUB_DIRS][MAX_PATH_LEN]);

int find_store_dirs(char store_dirs[][MAX_PATH_LEN]);

void read_item_data(char item_path[], char item_name[], float *item_price, float *item_score, int *item_entity);

#endif
