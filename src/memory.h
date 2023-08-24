#ifndef MEMORY_H_
#define MEMORY_H_

#include "common.h"
#include "value.h"

#define ALLOCATE(type, count) \
    (type *)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, ptr) reallocate(ptr, sizeof(type), 0)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(type, ptr, old_count, new_count)     \
    (type *)reallocate(ptr, sizeof(type) * (old_count), \
                       sizeof(type) * (new_count))

#define FREE_ARRAY(type, ptr, old_count) \
    reallocate(ptr, sizeof(type) * (old_count), 0)

void *reallocate(void *ptr, size_t old_size, size_t new_size);
void mark_object(obj_t *object);
void mark_value(value_t value);
void collect_garbage();
void free_objects();

#endif// MEMORY_H_
