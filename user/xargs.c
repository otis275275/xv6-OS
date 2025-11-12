#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  char buf[512];
  char *args[32];
  int n = 0;

  // copy fixed args (program to run and its fixed args)
  if (argc < 2) {
    // nothing to run
    exit(0);
  }
  for (int i = 1; i < argc; i++) {
    args[n++] = argv[i];
  }

  while (1) {
    int len = 0;
    int r;
    char c;

    // read one line from stdin, char by char
    while ((r = read(0, &c, 1)) == 1) {
      if (c == '\n' || c == '\r') {
        break;
      }
      if (len < (int)sizeof(buf) - 1) {
        buf[len++] = c;
      }
      // else: truncate long line silently
    }

    if (r == 0 && len == 0) {
      // EOF and no data -> done
      break;
    }

    // terminate string
    buf[len] = 0;

    // if empty line (len==0) skip
    if (len == 0) {
      if (r == 0) break; // EOF after empty line
      continue;
    }

    // prepare argv for exec: fixed args + this line
    args[n] = buf;
    args[n + 1] = 0;

    int pid = fork();
    if (pid < 0) {
      // fork failed
      fprintf(2, "xargs: fork failed\n");
      break;
    }
    if (pid == 0) {
      // child: execute
      exec(args[0], args);
      // if exec returns, error
      fprintf(2, "xargs: exec %s failed\n", args[0]);
      exit(1);
    } else {
      // parent: wait for child to finish
      wait(0);
    }

    // if EOF was encountered (r == 0) then break loop
    if (r == 0) break;
    // otherwise continue to read next line (buf will be overwritten)
  }

  exit(0);
}
