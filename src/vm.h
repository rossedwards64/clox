#ifndef VM_H_
#define VM_H_

#include "chunk.h"
#include "common.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define FRAMES_MAX 64
#define STACK_MAX  (FRAMES_MAX * UINT8_COUNT)

typedef struct {
    obj_closure_t *closure;
    uint8_t *ip;
    value_t *slots;
} call_frame_t;

typedef struct {
    call_frame_t frames[FRAMES_MAX];
    int frame_count;
    value_t stack[STACK_MAX];
    value_t *stack_top;
    table_t globals;
    table_t strings;
    obj_str_t *init_string;
    obj_upvalue_t *open_upvalues;
    size_t bytes_allocated;
    size_t next_gc;
    obj_t *objects;
    int gray_count;
    int gray_capacity;
    obj_t **gray_stack;
} vm_t;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} interpret_result_t;

extern vm_t vm;

void init_vm();
void free_vm();

interpret_result_t interpret(const char *source);

void push(value_t value);
value_t pop();

#endif// VM_H_
