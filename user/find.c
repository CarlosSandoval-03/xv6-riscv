#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *fmtname(char *filename, char *path) {
  static char buf[512];

  // <path> + filename
  if (strlen(filename) + strlen(path) >= 512) {
    fprintf(2, "fmt: Error name too long\n");
    exit(1);
  }

  strcpy(buf, path);
  char *p = buf + strlen(buf);
  *p++ = '/';

  memmove(p, filename, sizeof(filename));
  return buf;
}

void find(char *path, char *pattern) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if (strlen(path) == 0) {
    return;
  }

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_FILE:
    break;
  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      fprintf(2, "find: path too long\n");
      exit(1);
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;

      if (strcmp(pattern, de.name) == 0) {
        memmove(p, de.name, DIRSIZ);
        printf("%s\n", fmtname(p, path));
        continue;
      }

      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
        continue;
      }

      char new_path[512];
      memmove(new_path, path, strlen(path) + 1);
      memmove(new_path + strlen(path), "/", 2);
      memmove(new_path + strlen(path) + 1, de.name, DIRSIZ);

      find(new_path, pattern);
    }
    break;
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: find <directory> <pattern>\n");
    exit(0);
  }
  find(argv[1], argv[2]);

  exit(0);
}
