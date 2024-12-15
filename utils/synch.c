#include "../headers/synch.h"

#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

atomic_int lock = 0;
atomic_int shopping_lock = 0;
atomic_int file_lock = 0;
atomic_int user_lock = 0;
atomic_int category_lock = 0;
atomic_int order_lock = 0;
atomic_int msq_lock = 0;
atomic_int update_entity_lock = 0;
atomic_int enter_score_lock = 0;
atomic_int update_score_lock = 0;

int test_and_set(atomic_int* lock) {
    return atomic_exchange(lock, 1);
}

void enter_critical_section(atomic_int* lock) {
    while (test_and_set(lock) == 1);
}

void exit_critical_section(atomic_int* lock) {
    atomic_store(lock, 0);
}