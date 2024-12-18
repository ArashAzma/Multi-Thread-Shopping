#ifndef SYNCH_H
#define SYNCH_H

#include <stdatomic.h>

extern atomic_int lock;
extern atomic_int shopping_lock;
extern atomic_int file_lock;
extern atomic_int user_lock;
extern atomic_int category_lock;
extern atomic_int order_lock;
extern atomic_int msq_lock;
extern atomic_int update_entity_lock;
extern atomic_int enter_score_lock;
extern atomic_int update_score_lock;
extern atomic_int count_lock;
extern atomic_int order_lock2;

int test_and_set(atomic_int* lock);

void enter_critical_section(atomic_int* lock);

void exit_critical_section(atomic_int* lock);

#endif 