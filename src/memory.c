#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>
#include "debug.h"
#endif


#define GC_HEAP_GROW_FACTOR 2

static void free_object(obj_t *object);

void *reallocate(void *ptr, size_t old_size, size_t new_size)
{
    vm.bytes_allocated += new_size - old_size;
    if (new_size > old_size) {
#undef DEBUG_STRESS_GC
#ifdef DEBUG_STRESS_GC
        collect_garbage();
#else
        if (vm.bytes_allocated > vm.next_gc) {
            collect_garbage();
        }
#endif
    }

    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    void *result = realloc(ptr, new_size);
    if (result == NULL) exit(1);
    return result;
}

void mark_object(obj_t *object)
{
    if (object == NULL) return;
    if (object->is_marked) return;

#ifdef DEBUG_LOG_GC
    printf("%p mark ", (void *)object);
    print_value(OBJ_VAL(object));
    printf("\n");
#endif

    object->is_marked = true;

    if (vm.gray_capacity < vm.gray_count + 1) {
        vm.gray_capacity = GROW_CAPACITY(vm.gray_capacity);
        vm.gray_stack = (obj_t **)realloc(vm.gray_stack,
                                          sizeof(obj_t *) * vm.gray_capacity);
        if (vm.gray_stack == NULL) exit(1);
    }
    vm.gray_stack[vm.gray_count++] = object;
}

void mark_value(value_t value)
{
    if (IS_OBJ(value)) mark_object(AS_OBJ(value));
}

static void mark_array(value_array_t *array)
{
    for (int i = 0; i < array->count; i++) {
        mark_value(array->values[i]);
    }
}

static void blacken_object(obj_t *object)
{
#ifdef DEBUG_LOG_GC
    printf("%p blacken", (void *)object);
    print_value(OBJ_VAL(object));
    printf("\n");
#endif

    switch (object->type) {
        case OBJ_CLOSURE: {
            obj_closure_t *closure = (obj_closure_t *)object;
            mark_object((obj_t *)closure->function);
            for (int i = 0; i < closure->upvalue_count; i++) {
                mark_object((obj_t *)closure->upvalues[i]);
            }
            break;
        }
        case OBJ_FUNCTION: {
            obj_function_t *function = (obj_function_t *)object;
            mark_object((obj_t *)function->name);
            mark_array(&function->chunk.constants);
            break;
        }
        case OBJ_UPVALUE:
            mark_value(((obj_upvalue_t *)object)->closed);
        case OBJ_NATIVE:
        case OBJ_STRING:
            break;
    }
}

static void mark_roots();
static void trace_references();
static void sweep();

void collect_garbage()
{
#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
    size_t before = vm.bytes_allocated;
#endif

    mark_roots();
    trace_references();
    table_remove_white(&vm.strings);
    sweep();

    vm.next_gc = vm.bytes_allocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
    printf("-- gc end\n");
    printf("   collected %zu bytes (from %zu to %zu) next at %zu\n.",
           before - vm.bytes_allocated, before, vm.bytes_allocated,
           vm.next_gc);
#endif
}

static void mark_roots()
{
    for (value_t *slot = vm.stack; slot < vm.stack_top; slot++) {
        mark_value(*slot);
    }

    for (int i = 0; i < vm.frame_count; i++) {
        mark_object((obj_t *)vm.frames[i].closure);
    }

    for (obj_upvalue_t *upvalue = vm.open_upvalues;
         upvalue != NULL;
         upvalue = upvalue->next) {
        mark_object((obj_t *)upvalue);
    }

    mark_table(&vm.globals);
    mark_compiler_roots();
}

static void trace_references()
{
    while (vm.gray_count > 0) {
        obj_t *object = vm.gray_stack[--vm.gray_count];
        blacken_object(object);
    }
}

static void sweep()
{
    obj_t *previous = NULL;
    obj_t *object = vm.objects;
    while (object != NULL) {
        if (object->is_marked) {
            object->is_marked = false;
            previous = object;
            object = object->next;
        } else {
            obj_t *unreached = object;
            object = object->next;
            if (previous != NULL) {
                previous->next = object;
            } else {
                vm.objects = object;
            }

            free_object(unreached);
        }
    }
}

void free_objects()
{
    obj_t *object = vm.objects;
    while (object != NULL) {
        obj_t *next = object->next;
        free_object(object);
        object = next;
    }
    free(vm.gray_stack);
}

static void free_object(obj_t *object)
{
#ifdef DEBUG_LOG_GC
    printf("%p free type %d\n", (void *)object, object->type);
#endif

    switch (object->type) {
        case OBJ_CLOSURE: {
            obj_closure_t *closure = (obj_closure_t *) object;
            FREE_ARRAY(obj_upvalue_t *, closure->upvalues,
                       closure->upvalue_count);
            FREE(obj_closure_t, object);
            break;
        }
        case OBJ_FUNCTION: {
            obj_function_t *function = (obj_function_t *)object;
            free_chunk(&function->chunk);
            FREE(obj_function_t, object);
            break;
        }
        case OBJ_NATIVE: {
            FREE(obj_native_t, object);
            break;
        }
        case OBJ_STRING: {
            obj_str_t *string = (obj_str_t *)object;
            FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(obj_str_t, object);
            break;
        }
        case OBJ_UPVALUE: {
            FREE(obj_upvalue_t, object);
        }
    }
}
