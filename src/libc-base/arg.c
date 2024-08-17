// This code is released under the MIT License

#include <libc-base.h>

#define isspace(c)                                                                                 \
  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\f' || (c) == '\v')

static char *parse_arg(char *arg, cstr *argline_p) {
  cstr s = *argline_p;

  if (arg == null) arg = malloc(128);
  size_t len = malloc_usable_size(arg);
  size_t i   = 0;
#define d                                                                                          \
  *({                                                                                              \
    if (i >= len) {                                                                                \
      char *_arg = realloc(arg, len + 128);                                                        \
      if (_arg == null) goto err;                                                                  \
      arg = _arg;                                                                                  \
      len = malloc_usable_size(arg);                                                               \
    }                                                                                              \
    &arg[i++];                                                                                     \
  })

  for (; *s && !isspace(*s); s++) {
    if (*s == '\'' || *s == '\"') {
      char q = *s;
      for (s++; *s && *s != q; s++) {
        if (*s == '\\') {
          s++;
          if (*s == '\0') goto err;
        }
        d = *s;
      }
      if (*s == '\0') goto err;
    } else {
      if (*s == '\\') {
        s++;
        if (*s == '\0') goto err;
      }
      d = *s;
    }
  }
  d = '\0';

#undef d
  *argline_p = s;
  return arg;

err:
  free(arg);
  return null;
}

#if ARGS_NOT_IN_STACK
#  define PUSH(val)                                                                                \
    ({                                                                                             \
      (*((size_t *)sp++) = (size_t)(val));                                                         \
      sp - sizeof(size_t);                                                                         \
    })
#  define PUSHMEM(ptr, len)                                                                        \
    ({                                                                                             \
      memcpy(sp, ptr, len);                                                                        \
      sp += (len);                                                                                 \
      sp - (len);                                                                                  \
    })
#  define PUSHSTR(str) PUSHMEM(str, strlen(str) + 1)
#else
#  define PUSH(val) (*(--(size_t *)sp) = (size_t)(val))
#  define PUSHMEM(ptr, len)                                                                        \
    ({                                                                                             \
      sp -= (len);                                                                                 \
      memcpy(sp, ptr, len);                                                                        \
      sp;                                                                                          \
    })
#  define PUSHSTR(str) PUSHMEM(str, strlen(str) + 1)
#endif

dlexport void *parse_args(args_t args) {
  cstr  path     = args->path;
  cstr  cmdline  = args->cmdline;
  cstr  argline  = args->argline;
  cstr  envline  = args->envline;
  cstr *old_envp = args->old_envp;
  void *sp       = args->sp;

  size_t argv_len = 15;
  char **argv     = malloc((argv_len + 1) * sizeof(char *));

  char  *buf  = null;
  size_t argc = 1;

  if (cmdline == null) {
    argv[0] = PUSHSTR(path);
  } else {
    buf = parse_arg(buf, &cmdline);
    if (buf == null) return null;
    argv[0] = PUSHSTR(path ?: buf);
    argline = cmdline;
  }

  for (; isspace(*argline); argline++) {}
  for (; *argline != '\0'; argc++) {
    buf = parse_arg(buf, &argline);
    if (buf == null) return null;
    for (; isspace(*argline); argline++) {}
    if (argc >= argv_len) {
      argv_len += 16;
      argv      = realloc(argv, (argv_len + 1) * sizeof(char *));
    }
    argv[argc] = PUSHSTR(buf);
  }
  argv[argc + 1] = null;
  free(buf);

  args->argc = argc;
  args->argv = PUSHMEM(argv, (argc + 1) * sizeof(char *));
  free(argv);

  args->envp = PUSH(null);

  return args->sp = sp;
}

#undef PUSHSTR
#undef PUSHMEM
#undef PUSH

#undef isspace
