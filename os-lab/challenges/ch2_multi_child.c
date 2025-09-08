// src/ch2_multi_child.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s N\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "N must be > 0\n");
        return 1;
    }
    if (N > 254) {
        fprintf(stderr, "warning: only the low 8 bits of exit status are kept; "
                        "using N=%d may wrap\n", N);
    }

    pid_t *pids = malloc(sizeof(pid_t) * N);
    if (!pids) { perror("malloc"); return 1; }

    // Spawn N children
    for (int i = 0; i < N; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            // Child i
            printf("[child %d] PID=%d, PPID=%d\n", i, getpid(), getppid());
            // Exit code equal to index+1 (fits in 8 bits)
            _exit((i + 1) & 0xff);
        }
        // Parent: remember which PID belongs to which index
        pids[i] = pid;
    }

    // Parent waits for all children and reports finish order + exit status
    for (int done = 0; done < N; ++done) {
        int status = 0;
        pid_t w = wait(&status);
        if (w == -1) { perror("wait"); free(pids); return 1; }

        // Find the child's index from the PID we stored
        int idx = -1;
        for (int i = 0; i < N; ++i) {
            if (pids[i] == w) { idx = i; break; }
        }

        if (WIFEXITED(status)) {
            printf("[parent] child %d (PID=%d) exited with status %d\n",
                   idx, w, WEXITSTATUS(status));
        } else {
            printf("[parent] child %d (PID=%d) did not exit normally\n", idx, w);
        }
    }

    free(pids);
    return 0;
}

