#include "array_list.h"
#include "mallocr.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DEFINE_ARRAY_LIST_HEADER(int, int)
DEFINE_ARRAY_LIST_IMPL(int, int)

int main() {
  printf("Allocating array.\n");
  struct int_array_list_result result = int_array_list_create();
  if (!result.ok) {
    printf("Error creating array: %s\n", strerror(result.value.error));
    exit(EXIT_FAILURE);
  }
  struct int_array_list list = result.value.ok;

  printf("Adding items.\n");
  for (int i = 0; i < 128; i++) {
    struct realloc_result result = int_array_list_add(&list, i);
    if (!result.ok) {
      printf("Error adding item: %s\n", strerror(result.value.error));
      exit(EXIT_FAILURE);
    }
    printf("Array length: %d, array capacity: %d\n", list.length,
           list.capacity);
  }

  assert(list.length == 128);

  printf("[");
  for (int i = 0; i < list.length; i++) {
    struct int_option element = int_array_list_get(&list, i);
    if (!element.some) {
      perror("\nElement couldn't be found.\n");
      exit(EXIT_FAILURE);
    }
    printf("%d, ", element.value);
  }
  printf("]\n");

  int_array_list_free(&list);
  exit(EXIT_SUCCESS);
}
