#ifndef VM_H_
#define VM_H_

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    chunk_t *chunk;
    uint8_t *ip;
    value_t stack[STACK_MAX];
    value_t *stack_top;
} vm_t;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} interpret_result_t;

void init_vm();
void free_vm();

interpret_result_t interpret(chunk_t *chunk);

void push(value_t value);
value_t pop();

#endif // VM_H_