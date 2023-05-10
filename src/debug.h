#ifndef DEBUG_H_
#define DEBUG_H_

#include "chunk.h"

void disassemble_chunk(chunk_t *chunk, const char *name);
int disassemble_instruction(chunk_t *chunk, int offset);

#endif// DEBUG_H_
