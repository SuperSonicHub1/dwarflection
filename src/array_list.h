// A generic array list implementation in macros.
// Implements some pseudo-code from https://en.wikipedia.org/wiki/Dynamic_array
// The rest is from my memory of implementing one in AP CSA.
// Heavy use of C preprocessore concatenation here:
// https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html
// I've learned two things today:
// 1. I love macros.
// 2. I hate macros in C (and the language's general dearth of modern
// programming features).

#include "mallocr.h"
#include <stdlib.h>
#include <sys/types.h>

#pragma once

#define DEFINE_ARRAY_LIST_HEADER(prefix, value_type)                           \
  struct prefix##_array_list {                                                 \
    value_type *elements;                                                      \
    u_int32_t length;                                                          \
    u_int32_t capacity;                                                        \
  };                                                                           \
  struct prefix##_array_list_result DEFINE_RESULT(struct prefix##_array_list,  \
                                                  int);                        \
  struct prefix##_option DEFINE_OPTION(value_type);                            \
  struct prefix##_array_list_result prefix##_array_list_create();              \
  struct realloc_result prefix##_array_list_add(                               \
      struct prefix##_array_list *list, value_type element);                   \
  struct prefix##_option prefix##_array_list_get(                              \
      struct prefix##_array_list *list, u_int32_t index);                      \
  void prefix##_array_list_free(struct prefix##_array_list *list);

#define DEFINE_ARRAY_LIST_IMPL(prefix, value_type)                             \
  struct prefix##_array_list_result prefix##_array_list_create() {             \
    struct prefix##_array_list_result result;                                  \
    struct prefix##_array_list list;                                           \
    list.length = 0;                                                           \
    /* Default size of array will be 32. */                                    \
    list.capacity = 32;                                                        \
    struct malloc_result allocation =                                          \
        mallocr(sizeof(value_type) * list.capacity);                           \
    if (!allocation.ok) {                                                      \
      ERR(result, allocation.value.error);                                     \
      return result;                                                           \
    }                                                                          \
    list.elements = (value_type *)allocation.value.ok;                         \
    OK(result, list);                                                          \
    return result;                                                             \
  }                                                                            \
                                                                               \
  struct realloc_result prefix##_array_list_add(                               \
      struct prefix##_array_list *list, value_type element) {                  \
    struct realloc_result allocation;                                          \
    if (list->length == list->capacity) {                                      \
      list->capacity *= 2;                                                     \
      allocation =                                                             \
          reallocr(list->elements, sizeof(value_type) * list->capacity);       \
      /* Bail early if stuff goes wrong. */                                    \
      if (!allocation.ok)                                                      \
        return allocation;                                                     \
    }                                                                          \
    list->elements[list->length] = element;                                    \
    list->length++;                                                            \
    OK(allocation, list->elements);                                            \
    return allocation;                                                         \
  }                                                                            \
                                                                               \
  struct prefix##_option prefix##_array_list_get(                              \
      struct prefix##_array_list *list, u_int32_t index) {                     \
    struct prefix##_option result;                                             \
    if (list->length == 0) {                                                   \
      NONE(result);                                                            \
      return result;                                                           \
    }                                                                          \
    SOME(result, list->elements[index]);                                       \
    return result;                                                             \
  }                                                                            \
  void prefix##_array_list_free(struct prefix##_array_list *list) {            \
    free(list->elements);                                                      \
  }
