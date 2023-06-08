#ifndef OBJECT_H_
#define OBJECT_H_

#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_STRING(value) is_obj_type(value, OBJ_STRING)

#define AS_STRING(value) ((obj_str_t *)AS_OBJ(value))
#define AS_CSTRING(value) (((obj_str_t *)AS_OBJ(value))->chars)

typedef enum {
    OBJ_STRING,
} obj_type_t;

struct obj_t {
    obj_type_t type;
    struct obj_t *next;
};

struct obj_str_t {
    obj_t obj;
    int length;
    char *chars;
};

obj_str_t *take_string(char *chars, int length);
obj_str_t *copy_string(const char *chars, int length);
void print_object(value_t value);

static inline bool is_obj_type(value_t value, obj_type_t type)
{
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif// OBJECT_H_
