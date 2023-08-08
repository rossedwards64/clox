#ifndef COMPILER_H_
#define COMPILER_H_

#include "chunk.h"
#include "object.h"

obj_function_t *compile(const char *source);

#endif// COMPILER_H_
