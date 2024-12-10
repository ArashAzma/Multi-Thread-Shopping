#include "../headers/synch.h"

#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

atomic_int lock = 0;
atomic_int order_lock = 0;

int test_and_set(atomic_int* lock) {
    return atomic_exchange(lock, 1);
}

void enter_critical_section(atomic_int* lock) {
    while (test_and_set(lock) == 1);
}

void exit_critical_section(atomic_int* lock) {
    atomic_store(lock, 0);
}