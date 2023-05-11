#ifndef CHUNK_H_
#define CHUNK_H_

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
} op_code;

typedef struct {
    int count;
    int capacity;
    uint8_t *code;
    int *lines;
    value_array constants;
} chunk_t;

void init_chunk(chunk_t *chunk);
void write_chunk(chunk_t *chunk, uint8_t byte, int line);
int add_constant(chunk_t *chunk, value_t value);
void free_chunk(chunk_t *chunk);

#endif// CHUNK_H_
