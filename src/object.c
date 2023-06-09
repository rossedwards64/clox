#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, object_type) \
    (type *)allocate_object(sizeof(type), object_type)

static obj_t *allocate_object(size_t size, obj_type_t type)
{
    obj_t *object = (obj_t *)reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

static obj_str_t *allocate_string(char *chars, int length,
                                  uint32_t hash)
{
    obj_str_t *string = ALLOCATE_OBJ(obj_str_t, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    table_set(&vm.strings, string, NIL_VAL);
    return string;
}

static uint32_t hash_string(const char *key, int length)
{
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

obj_str_t *take_string(char *chars, int length)
{
    uint32_t hash = hash_string(chars, length);
    obj_str_t *interned = table_find_string(&vm.strings, chars,
                                            length, hash);
    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }
    return allocate_string(chars, length, hash);
}

obj_str_t *copy_string(const char *chars, int length)
{
    uint32_t hash = hash_string(chars, length);
    obj_str_t *interned = table_find_string(&vm.strings, chars,
                                            length, hash);
    if (interned != NULL) return interned;
    char *heap_chars = ALLOCATE(char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    return allocate_string(heap_chars, length, hash);
}

void print_object(value_t value)
{
    switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
        printf("%s", AS_CSTRING(value));
        break;
    }
}
