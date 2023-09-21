#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define STDIN 0
#define MAX_LEN_STR 512
#define LIMIT_ITERATION 1e09

int iteration_count = 0;

void *create_buffer(uint size_in_bytes) {
  char *buf = (void *)malloc(size_in_bytes);
  if (buf == 0) {
    fprintf(2, "ERROR: Something went wrong with malloc\n");
    exit(1);
  }
  memset(buf, 0, size_in_bytes);

  return buf;
}

int get_empty_index_argv(char **argv) {
  for (int i = 0; i < MAXARG; i++) {
    if (*(argv + i) == 0) {
      return i;
    }
  }
  return -1;
}

void save_argument(char **argv_dst, char *string) {
  if (strlen(string) == 0) {
    return;
  }

  char *arg = (char *)create_buffer((strlen(string) * sizeof(char)) + 1);
  strcpy(arg, string);

  int index = get_empty_index_argv(argv_dst);
  if (index < 0) {
    fprintf(2, "ERROR: Unable to save the indicated argument\n");
    exit(1);
  }

  *(argv_dst + index) = arg;
}

void read_save_arguments(char **argv) {
  char c = '\0';
  char *buf_word = (char *)create_buffer(MAX_LEN_STR * sizeof(char)),
       *p = buf_word;

  iteration_count = 0;
  while (read(STDIN, &c, 1) != 0 && iteration_count < LIMIT_ITERATION) {
    iteration_count++;

    if (c == '\'' || c == '"')
      continue;

    if (c != '\n' && c != ' ' && c != '\r' && c != '\\') {
      *p++ = c;
      continue;
    }

    if (c == '\\')
      read(STDIN, &c, 1);

    *p = '\0';
    save_argument(argv, buf_word);

    p = buf_word;
    memset(buf_word, 0, MAX_LEN_STR);
  }

  if (iteration_count == LIMIT_ITERATION) {
    fprintf(2, "ERROR: Iteration limit reached\n");
    exit(1);
  }
}

int main(int argc, char **argv) {
  int index = 1, max_args = -1;

  if (strcmp("-n", *(argv + 1)) == 0) {
    if (argc < 3) {
      fprintf(2, "xargs: option requires an argument -- 'n'\n");
      fprintf(2, "Usage: xargs -n <num max arguments> <args...>\n");
      exit(1);
    }

    char *max_args_str = *(argv + 2);
    max_args = atoi(max_args_str);
    if (max_args <= 0) {
      fprintf(2, "xargs: value 0 for -n option should be >= 1\n");
      fprintf(2, "Usage: xargs -n <num max arguments> <args...>\n");
      exit(1);
    }

    index = 3;
  }

  char **new_argv = (char **)create_buffer(MAXARG * sizeof(char *));
  memmove(new_argv, (argv + index), (sizeof(char *) * (argc - index)));

  if (max_args < 0) {
    read_save_arguments(new_argv);
    exec(*new_argv, new_argv);
    fprintf(2, "ERROR: The indicated instruction could not be executed "
               "(NORMAL_EXEC)\n");
    exit(1);
  }

  // If user set -n option
  char **stdin_argv =
      (char **)create_buffer((MAXARG - argc + index) * sizeof(char *));
  read_save_arguments(stdin_argv);

  iteration_count = 0;
  int limit_stdin = get_empty_index_argv(stdin_argv), stdin_iterable = 0;
  while (stdin_iterable < limit_stdin && iteration_count < LIMIT_ITERATION) {
    iteration_count++;

    uint argv_size = MAXARG * sizeof(char *);
    char **child_argv = (char **)create_buffer(argv_size);
    memmove(child_argv, new_argv, argv_size);
    int start = get_empty_index_argv(new_argv);

    for (int i = 0; i < max_args; i++) {
      memmove((child_argv + start + i), (stdin_argv + stdin_iterable),
              sizeof(char *));

      stdin_iterable++;
    }

    int pid = fork(), status = 0;
    if (pid > 0) {
      wait(&status);

      if (status != 0) {
        fprintf(2, "ERROR: Exec on child\n");
        exit(1);
      }
    } else if (pid == 0) {
      exec(*child_argv, child_argv);
      fprintf(2, "ERROR: Something was wrong on exec (CHILD_EXEC)\n");
      exit(1);
    } else {
      fprintf(2, "ERROR: Something was wrong on fork the process\n");
      exit(1);
    }
  }

  exit(0);
}
