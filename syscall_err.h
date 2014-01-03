#ifndef SYSCALL_ERR_H_
# define SYSCALL_ERR_H_

/*
 * Yay! GNU extensions!
 *
 * These macros simplify error management a little bit by factor a very common
 * code pattern: call a syscall that takes a path as a first argument and
 * returns an int, check the return value, and err/warn on this out if the
 * return is -1, otherwise use the value.
 */

# include <err.h>

# define SYSCALL_ERR(SYSCALL, PATH, ...) ({                                   \
  int __warn_syscall_res;                                                     \
  if ((__warn_syscall_res = SYSCALL(PATH, ##__VA_ARGS__)) == -1)              \
    err(EXIT_FAILURE, "%s: %d: " #SYSCALL "(%s)", __FILE__, __LINE__, PATH);  \
  __warn_syscall_res;                                                         \
})

# define SYSCALL_WARN(SYSCALL, PATH, ...) ({                      \
  int __warn_syscall_res;                                         \
  if ((__warn_syscall_res = SYSCALL(PATH, ##__VA_ARGS__)) == -1)  \
    warn("%s: %d: " #SYSCALL "(%s)", __FILE__, __LINE__, PATH);   \
  __warn_syscall_res;                                             \
})

#endif /* !SYSCALL_ERR_H_ */
