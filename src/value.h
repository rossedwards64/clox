#ifndef VALUE_H_
#define VALUE_H_

#include "common.h"

typedef double value_t;

typedef struct {
    int capacity;
    int count;
    value_t *values;
} value_array;

void init_value_array(value_array *array);
void write_value_array(value_array *array, value_t value);
void free_value_array(value_array *array);

void print_value(value_t value);

#endif // VALUE_H_
