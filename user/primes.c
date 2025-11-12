#include "kernel/types.h"
#include "user/user.h"

// Khai báo 'noreturn' để tránh lỗi 'infinite recursion'
void sieve(int) __attribute__((noreturn));

void sieve(int left_pipe_read_end) {
    int prime;
    int n;

    // 1. Đọc số nguyên tố đầu tiên
    if (read(left_pipe_read_end, &prime, sizeof(int)) == 0) {
        // 2. Trường hợp cơ sở: Không còn số, thoát
        close(left_pipe_read_end);
        exit(0);
    }

    // 3. In số nguyên tố
    printf("prime %d\n", prime);

    // 4. Tạo đường ống bên phải và quy trình con
    int right_pipe[2];
    pipe(right_pipe);

    if (fork() == 0) {
        // 5. --- Quy trình con (Tầng lọc N+1) ---
        close(right_pipe[1]); // Con không cần ghi
        close(left_pipe_read_end); // Con không cần đọc ống bên trái của cha

        // Đệ quy
        sieve(right_pipe[0]);

        // Dọn dẹp
        close(right_pipe[0]);
        exit(0); // Sửa lỗi: Phải có tham số 0

    } else {
        // 6. --- Quy trình cha (Tầng lọc N) ---
        close(right_pipe[0]); // Cha không cần đọc

        // 7. Lọc và chuyển tiếp số
        while (read(left_pipe_read_end, &n, sizeof(int)) > 0) {
            if (n % prime != 0) {
                write(right_pipe[1], &n, sizeof(int));
            }
        }

        // 8. Dọn dẹp
        close(left_pipe_read_end);
        close(right_pipe[1]);

        // 9. Đợi con hoàn thành
        wait(0); 
        exit(0); 
    }
}

// Hàm main: Bắt đầu pipeline
int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // --- Quy trình con (Tầng lọc đầu tiên) ---
        close(p[1]); 
        sieve(p[0]); 
        close(p[0]);
        exit(0); 
    } else {
        // --- Quy trình cha (Quy trình 'main') ---
        close(p[0]); 

        // 1. Bơm số 2-280 vào đường ống
        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(int));
        }

        // 2. Đóng đầu ghi
        close(p[1]);

        // 3. Đợi con hoàn thành
        wait(0); 
        exit(0); 
    }
}