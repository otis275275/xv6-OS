#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *filename) {
  char buf[512], *p; // buffer to build full path
  int fd; // file descriptor for the current directory
  struct dirent de; // to store directory entries (files/subfolders)
  struct stat st; // to store file info (type, size, etc.)

  // open directory
  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  // get info about the file/directory
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // if it is FILE -> STOP
  if (st.type == T_FILE) {
    close(fd);
    return;
  }

  // check if the new full path (current path + "/" + filename) will fit in the buffer
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    printf("find: path too long\n");
    close(fd);
    return;
  }

  // copy the current path into the buffer
  strcpy(buf, path);
  p = buf + strlen(buf); // move pointer 'p' to the end of the path
  *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0) // empty entry
      continue;

    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) 
      continue; // skip "." and ".." to avoid infinite recursion

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (stat(buf, &st) < 0)
      continue;

    // if it's a file and the name matches -> print the full path
    if (st.type == T_FILE && strcmp(de.name, filename) == 0)
      printf("%s\n", buf);
    else if (st.type == T_DIR)
      find(buf, filename);
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find <path> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
