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

#ifdef DEBUG_LOG_GC
    printf("%p allocate %zu for %d\n", (void *)object, size, type);
#endif

    return object;
}

obj_class_t *new_class(obj_str_t *name)
{
    obj_class_t *klass = ALLOCATE_OBJ(obj_class_t, OBJ_CLASS);
    klass->name = name;
    return klass;
}

obj_closure_t *new_closure(obj_function_t *function)
{
    obj_upvalue_t **upvalues = ALLOCATE(obj_upvalue_t *,
                                        function->upvalue_count);
    for (int i = 0; i < function->upvalue_count; i++) {
        upvalues[i] = NULL;
    }
    obj_closure_t *closure = ALLOCATE_OBJ(obj_closure_t, OBJ_CLOSURE);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalue_count = function->upvalue_count;
    return closure;
}

obj_function_t *new_function()
{
    obj_function_t *function = ALLOCATE_OBJ(obj_function_t, OBJ_FUNCTION);
    function->arity = 0;
    function->upvalue_count = 0;
    function->name = NULL;
    init_chunk(&function->chunk);
    return function;
}

obj_instance_t *new_instance(obj_class_t *klass)
{
    obj_instance_t *instance = ALLOCATE_OBJ(obj_instance_t, OBJ_INSTANCE);
    instance->klass = klass;
    init_table(&instance->fields);
    return instance;
}

obj_native_t *new_native(native_fn_t function)
{
    obj_native_t *native = ALLOCATE_OBJ(obj_native_t, OBJ_NATIVE);
    native->function = function;
    return native;
}

static obj_str_t *allocate_string(char *chars, int length,
                                  uint32_t hash)
{
    obj_str_t *string = ALLOCATE_OBJ(obj_str_t, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    push(OBJ_VAL(string));
    table_set(&vm.strings, string, NIL_VAL);
    pop();
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

obj_upvalue_t *new_upvalue(value_t *slot)
{
    obj_upvalue_t *upvalue = ALLOCATE_OBJ(obj_upvalue_t, OBJ_UPVALUE);
    upvalue->location = slot;
    upvalue->closed = NIL_VAL;
    upvalue->next = NULL;
    return upvalue;
}

static void print_function(obj_function_t *function)
{
    if (function->name == NULL) {
        printf("<script>");
        return;
    }
    printf("<fn %s>", function->name->chars);
}

void print_object(value_t value)
{
    switch (OBJ_TYPE(value)) {
        case OBJ_CLASS:
            printf("%s", AS_CLASS(value)->name->chars);
            break;
        case OBJ_CLOSURE:
            print_function(AS_CLOSURE(value)->function);
            break;
        case OBJ_FUNCTION:
            print_function(AS_FUNCTION(value));
            break;
        case OBJ_INSTANCE:
            printf("%s instance",
                   AS_INSTANCE(value)->klass->name->chars);
            break;
        case OBJ_NATIVE:
            printf("<native fn>");
            break;
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
        case OBJ_UPVALUE:
            printf("upvalue");
            break;
    }
}
