#include "kernel/types.h"
#include "user/user.h"

void remove_numbers(int ng_readfd) {
  int prime = 0;
  if (read(ng_readfd, &prime, sizeof(prime)) <= 0) {
    // Empty pipe
    exit(1);
  }
  printf("prime %d\n", prime);

  // Create aux pipe
  int aux[2];
  if (pipe(aux) < 0) {
    printf("Error creating pipe\n");
    exit(1);
  }

  int n = 0;
  while (read(ng_readfd, &n, sizeof(n)) > 0) {
    if (n % prime == 0)
      continue;

    write(aux[1], (const void *)&n, sizeof(n));
  }

  close(aux[1]);
  int pid = fork();
  if (pid > 0) {
    close(aux[0]);
    wait(0);
    exit(0);
  } else if (pid == 0) {
    remove_numbers(aux[0]);

    close(aux[0]);
    exit(0);
  } else {
    printf("Error fork\n");
    exit(1);
  }
}

int main() {
  int primes[2];
  if (pipe(primes) < 0) {
    printf("Error creating pipe\n");
    exit(1);
  }

  int pid = fork();
  if (pid > 0) {
    close(primes[0]);

    for (int i = 2; i <= 35; i++)
      write(primes[1], (const void *)&i, sizeof(i));

    close(primes[1]);

    int status = 0;
    wait(&status);
    exit(status);
  } else if (pid == 0) {
    close(primes[1]);

    remove_numbers(primes[0]);

    close(primes[0]);
  } else {
    printf("Error fork\n");
    exit(1);
  }

  exit(0);
}
