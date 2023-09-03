#ifndef OBJECT_H_
#define OBJECT_H_

#include "chunk.h"
#include "table.h"
#include "value.h"

#define OBJ_TYPE(value)    (AS_OBJ(value)->type)

#define IS_BOUND_METHOD(value) is_obj_type(value, OBJ_BOUND_TYPE)
#define IS_CLASS(value)        is_obj_type(value, OBJ_CLASS)
#define IS_CLOSURE(value)      is_obj_type(value, OBJ_CLOSURE)
#define IS_INSTANCE(value)     is_obj_type(value, OBJ_INSTANCE)
#define IS_FUNCTION(value)     is_obj_type(value, OBJ_FUNCTION)
#define IS_NATIVE(value)       is_obj_type(value, OBJ_NATIVE)
#define IS_STRING(value)       is_obj_type(value, OBJ_STRING)

#define AS_BOUND_METHOD(value) ((obj_bound_method_t *)AS_OBJ(value))
#define AS_CLASS(value)        ((obj_class_t *)AS_OBJ(value))
#define AS_CLOSURE(value)      ((obj_closure_t *)AS_OBJ(value))
#define AS_INSTANCE(value)     ((obj_instance_t *)AS_OBJ(value))
#define AS_FUNCTION(value)     ((obj_function_t *)AS_OBJ(value))
#define AS_NATIVE(value)   \
    (((obj_native_t *)AS_OBJ(value))->function)
#define AS_STRING(value)       ((obj_str_t *)AS_OBJ(value))
#define AS_CSTRING(value)      (((obj_str_t *)AS_OBJ(value))->chars)

typedef enum {
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_NATIVE,
    OBJ_STRING,
    OBJ_UPVALUE,
} obj_type_t;

struct obj_t {
    obj_type_t type;
    bool is_marked;
    struct obj_t *next;
};

typedef struct {
    obj_t obj;
    int arity;
    int upvalue_count;
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

typedef struct obj_upvalue_t {
    obj_t obj;
    value_t *location;
    value_t closed;
    struct obj_upvalue_t *next;
} obj_upvalue_t;

typedef struct {
    obj_t *obj;
    obj_function_t *function;
    obj_upvalue_t **upvalues;
    int upvalue_count;
} obj_closure_t;

typedef struct {
    obj_t obj;
    obj_str_t *name;
    table_t methods;
} obj_class_t;

typedef struct {
    obj_t obj;
    obj_class_t *klass;
    table_t fields;
} obj_instance_t;

typedef struct {
    obj_t obj;
    value_t receiver;
    obj_closure_t *method;
} obj_bound_method_t;

obj_bound_method_t *new_bound_method(value_t receiver,
                                     obj_closure_t *method);
obj_class_t *new_class(obj_str_t *name);
obj_closure_t *new_closure(obj_function_t *function);
obj_function_t *new_function();
obj_instance_t *new_instance(obj_class_t *klass);
obj_native_t *new_native(native_fn_t function);
obj_str_t *take_string(char *chars, int length);
obj_str_t *copy_string(const char *chars, int length);
obj_upvalue_t *new_upvalue(value_t *slot);
void print_object(value_t value);

static inline bool is_obj_type(value_t value, obj_type_t type)
{
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif// OBJECT_H_
