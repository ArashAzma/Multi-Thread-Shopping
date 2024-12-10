#ifndef SYNCH_H
#define SYNCH_H

#include <stdatomic.h>

extern atomic_int lock;
extern atomic_int order_lock;

int test_and_set(atomic_int* lock);

void enter_critical_section(atomic_int* lock);

void exit_critical_section(atomic_int* lock);

#endif 