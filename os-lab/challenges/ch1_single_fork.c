// src/ch1_single_fork.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Child
        printf("Hello from child (PID=%d, PPID=%d)\n", getpid(), getppid());
        sleep(2);
        _exit(7);                   // exit code 7 (use _exit in child)
    } else {
        // Parent
        int status = 0;
        pid_t w = waitpid(pid, &status, 0);
        if (w == -1) { perror("waitpid"); return 1; }

        if (WIFEXITED(status)) {
            printf("child %d exited with status %d\n", w, WEXITSTATUS(status));
        } else {
            printf("child %d did not exit normally\n", w);
        }
    }
    return 0;
}
