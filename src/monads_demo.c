#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include "monads.h"

// Demonstrate use of Result

enum division_error {
  DIVIDE_BY_ZERO,
};

struct division_result DEFINE_RESULT(double, enum division_error);

struct division_result divide(double a, double b) {
  struct division_result result;

  if (b == 0)
    ERR(result, DIVIDE_BY_ZERO);
  else
    OK(result, a / b);

  return result;
}

void display_division_result(struct division_result result) {
  if (result.ok) {
    printf("OK(%f)\n", result.value.ok);
  } else {
    switch (result.value.error) {
    case DIVIDE_BY_ZERO:
      printf("ERR(DIVIDE_BY_ZERO)\n");
      break;
    }
  }
}

// Demonstrate a more practical use of Result
extern int errno;

struct opendir_result DEFINE_RESULT(DIR*, int);

struct opendir_result opendirr(const char *dirname) {
  struct opendir_result result;

  DIR* directory = opendir(dirname);
  if (directory == NULL) ERR(result, errno);
  else OK(result, directory);

  return result;
}

int main() {
  display_division_result(divide(10, 5));
  display_division_result(divide(10, 0));
}
