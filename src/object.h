#ifndef OBJECT_H_
#define OBJECT_H_

#include "chunk.h"
#include "value.h"

#define OBJ_TYPE(value)    (AS_OBJ(value)->type)

#define IS_FUNCTION(value) is_obj_type(value, OBJ_FUNCTION)
#define IS_NATIVE(value)   is_obj_type(value, OBJ_NATIVE)
#define IS_STRING(value)   is_obj_type(value, OBJ_STRING)

#define AS_FUNCTION(value) ((obj_function_t *)AS_OBJ(value))
#define AS_NATIVE(value)   \
    (((obj_native_t *)AS_OBJ(value))->function)
#define AS_STRING(value)   ((obj_str_t *)AS_OBJ(value))
#define AS_CSTRING(value)  (((obj_str_t *)AS_OBJ(value))->chars)

typedef enum {
    OBJ_FUNCTION,
    OBJ_NATIVE,
    OBJ_STRING,
} obj_type_t;

struct obj_t {
    obj_type_t type;
    struct obj_t *next;
};

typedef struct {
    obj_t obj;
    int arity;
    chunk_t chunk;
    obj_str_t *name;
} obj_function_t;

typedef value_t (*native_fn_t)(int arg_count, value_t *args);

typedef struct {
    obj_t obj;
    native_fn_t function;
} obj_native_t;

struct obj_str_t {
    obj_t obj;
    int length;
    char *chars;
    uint32_t hash;
};

obj_function_t *new_function();
obj_native_t *new_native(native_fn_t function);
obj_str_t *take_string(char *chars, int length);
obj_str_t *copy_string(const char *chars, int length);
void print_object(value_t value);

static inline bool is_obj_type(value_t value, obj_type_t type)
{
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif// OBJECT_H_
