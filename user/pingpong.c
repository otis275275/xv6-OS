#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    //p1: cha sang con
    //p2: con sang cha
    int p1[2], p2[2];
    char buf[1];

    pipe(p1);
    pipe(p2);

    int pid = fork();

    if (pid == 0) {  // Tiến trình con
        close(p1[1]); // đóng đầu ghi của p1
        close(p2[0]); // đóng đầu đọc của p2

        read(p1[0], buf, 1);
        printf("%d: received ping\n", getpid());

        write(p2[1], buf, 1);

        close(p1[0]);
        close(p2[1]);
        exit(0);

    } else if (pid > 0) {  // Tiến trình cha
        close(p1[0]); // đóng đầu đọc của p1
        close(p2[1]); // đóng đầu ghi của p2

        char byte = 'x';
        write(p1[1], &byte, 1);

        read(p2[0], buf, 1);
        printf("%d: received pong\n", getpid());

        close(p1[1]);
        close(p2[0]);
        exit(0);
    } else {
        fprintf(2, "Fork failed\n");
        exit(1);
    }
}
