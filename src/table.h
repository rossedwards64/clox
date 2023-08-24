#ifndef TABLE_H_
#define TABLE_H_

#include <stdint.h>

#include "common.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

typedef struct {
    obj_str_t *key;
    value_t value;
} entry_t;

typedef struct {
    int count;
    int capacity;
    entry_t *entries;
} table_t;

void init_table(table_t *table);
void free_table(table_t *table);
bool table_get(table_t *table, obj_str_t *key, value_t *value);
bool table_set(table_t *table, obj_str_t *key, value_t value);
bool table_delete(table_t *table, obj_str_t *key);
void table_add_all(table_t *from, table_t *to);
obj_str_t *table_find_string(table_t *table, const char *chars,
                             int length, uint32_t hash);
void table_remove_white(table_t *table);
void mark_table(table_t *table);

#endif// TABLE_H_
