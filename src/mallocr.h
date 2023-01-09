#include <stddef.h>
#include "monads.h"

#pragma once

struct malloc_result DEFINE_RESULT(void*, int);
struct malloc_result mallocr(size_t size);

struct realloc_result DEFINE_RESULT(void*, int);
struct realloc_result reallocr(void *ptr, size_t size);

