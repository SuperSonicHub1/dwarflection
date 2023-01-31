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
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>

#pragma once

#define nearest_power_of_2(x) (u_int32_t) pow(ceil(log2(x)), 2)

#define DEFINE_ARRAY_LIST_HEADER_FULL_NAME_CONTROL(name, option_prefix,        \
                                                   value_type)                 \
  struct name {                                                                \
    value_type *elements;                                                      \
    u_int32_t length;                                                          \
    u_int32_t capacity;                                                        \
  };                                                                           \
  struct name##_result DEFINE_RESULT(struct name, int);                        \
  struct option_prefix##_option DEFINE_OPTION(value_type);                     \
  struct name##_result name##_create();                                        \
  struct name##_result name##_create_from_array(value_type* array);                          \
  struct realloc_result name##_reserve(struct name *list,                      \
                                       u_int32_t new_capacity);                \
  struct realloc_result name##_add(struct name *list, value_type element);     \
  struct option_prefix##_option name##_get(struct name *list,                  \
                                           u_int32_t index);                   \
  void name##_append(struct name *list, u_int32_t index);                      \
  void name##_free(struct name *list);

#define DEFINE_ARRAY_LIST_IMPL_FULL_NAME_CONTROL(name, option_prefix,          \
                                                 value_type)                   \
  struct name##_result name##_create() {                                       \
    struct name##_result result;                                               \
    struct name list;                                                          \
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
  struct realloc_result name##_reserve(struct name *list,                      \
                                       u_int32_t new_capacity) {               \
    assert(new_capacity >= list->capacity);                                    \
    struct realloc_result result =                                             \
        reallocr(list->elements, sizeof(value_type) * list->capacity);         \
    if (result.ok) {                                                           \
      list->capacity = new_capacity;                                           \
    }                                                                          \
    return result;                                                             \
  }                                                                            \
                                                                               \
  struct realloc_result name##_add(struct name *list, value_type element) {    \
    struct realloc_result allocation;                                          \
    if (list->length == list->capacity) {                                      \
      allocation = name##_reserve(list, nearest_power_of_2(list->capacity) * 2);                   \
      /* Bail early if stuff goes wrong. */                                    \
      if (!allocation.ok)                                                      \
        return allocation;                                                     \
      list->capacity *= 2;                                                     \
    }                                                                          \
    list->elements[list->length] = element;                                    \
    list->length++;                                                            \
    OK(allocation, list->elements);                                            \
    return allocation;                                                         \
  }                                                                            \
                                                                               \
  struct option_prefix##_option name##_get(struct name *list,                  \
                                           u_int32_t index) {                  \
    struct option_prefix##_option result;                                      \
    if (list->length == 0) {                                                   \
      NONE(result);                                                            \
      return result;                                                           \
    }                                                                          \
    SOME(result, list->elements[index]);                                       \
    return result;                                                             \
  }                                                                            \
  void name##_free(struct name *list) { free(list->elements); }

#define DEFINE_ARRAY_LIST_HEADER(prefix, option_prefix, value_type)            \
  DEFINE_ARRAY_LIST_HEADER_FULL_NAME_CONTROL(prefix##_array_list,              \
                                             option_prefix, value_type)
#define DEFINE_ARRAY_LIST_IMPL(prefix, option_prefix, value_type)              \
  DEFINE_ARRAY_LIST_IMPL_FULL_NAME_CONTROL(prefix##_array_list, option_prefix, \
                                           value_type)
