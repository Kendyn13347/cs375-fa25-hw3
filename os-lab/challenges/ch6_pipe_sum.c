// ch6_pipe_sum.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // --- Child: read from pipe, compute sum ---
        close(fd[1]); // close write end
        int sum = 0, num;
        while (read(fd[0], &num, sizeof(num)) > 0) {
            sum += num;
        }
        close(fd[0]);
        printf("Sum = %d\n", sum);
        _exit(0);
    } else {
        // --- Parent: write numbers 1..10 into pipe ---
        close(fd[0]); // close read end
        for (int i = 1; i <= 10; i++) {
            write(fd[1], &i, sizeof(i));
        }
        close(fd[1]); // signal EOF to child

        wait(NULL); // wait for child to finish
    }

    return 0;
}
