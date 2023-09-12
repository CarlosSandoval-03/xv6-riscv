#include "kernel/types.h"
#include "user/user.h"

int main() {
  int child_write_pipe[2];
  int father_write_pipe[2];

  int status;
  status = pipe(child_write_pipe);
  if (status < 0) {
    printf("Error on create pipe\n");
    exit(1);
  }

  status = pipe(father_write_pipe);
  if (status < 0) {
    printf("Error on create pipe\n");
    exit(1);
  }

  int pid = fork();
  if (pid > 0) {
    // Parent
    close(father_write_pipe[0]);
    close(child_write_pipe[1]);

    char buffer[1];
    write(father_write_pipe[1], (void *)buffer, 1);

    read(child_write_pipe[0], (void *)buffer, 1);
    printf("%d: received pong\n", getpid());

    close(father_write_pipe[1]);
    close(child_write_pipe[0]);
  } else if (pid == 0) {
    // Child
    close(child_write_pipe[0]);
    close(father_write_pipe[1]);

    char buffer[1];
    read(father_write_pipe[0], (void *)buffer, 1);
    printf("%d: received ping\n", getpid());

    write(child_write_pipe[1], (void *)buffer, 1);

    close(child_write_pipe[1]);
    close(father_write_pipe[0]);
  } else {
    printf("Error on fork the process\n");
    exit(1);
  }

  exit(0);
}
