// Implementations of Rust's most popular monads through macros
// and non-standard C extensions [1]. Makes use of statement
// expressions as seen in SerenityOS' TRY macro [2].
// [1]: https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
// [2]: https://github.com/SerenityOS/serenity/blob/master/AK/Try.h

#pragma once

#include <stdbool.h>

// Rust's Result: https://doc.rust-lang.org/std/result/enum.Result.html
#define DEFINE_RESULT(result_type, error_type)                                 \
  {                                                                            \
    bool ok;                                                                   \
    union {                                                                    \
      result_type ok;                                                          \
      error_type error;                                                        \
    } value;                                                                   \
  }

#define OK(variable, value_to_set)                                             \
  ({                                                                           \
    variable.ok = true;                                                        \
    variable.value.ok = value_to_set;                                          \
  })

#define ERR(variable, value_to_set)                                            \
  ({                                                                           \
    variable.ok = false;                                                       \
    variable.value.error = value_to_set;                                       \
  })

// Rust's Option: https://doc.rust-lang.org/std/option/enum.Option.html
#define DEFINE_OPTION(value_type)                                              \
  {                                                                            \
    bool some;                                                                 \
    value_type value;                                                          \
  }

#define SOME(variable, value_to_set)                                           \
  ({                                                                           \
    variable.some = true;                                                      \
    variable.value = value_to_set;                                             \
  })

#define NONE(variable) ({ variable.some = false; })
