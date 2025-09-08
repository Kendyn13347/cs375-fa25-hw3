// ch9_zombie.c
// Fork a child that exits immediately. Parent sleeps (default 10s)
// before calling wait() so the child becomes a zombie during the sleep.
// Pass --immediate to reap right away (no zombie window).

#define _XOPEN_SOURCE 700
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    int immediate = (argc > 1 && strcmp(argv[1], "--immediate") == 0);
    int sleep_secs = immediate ? 0 : 10;

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // Child: exit immediately so it becomes a zombie until reaped.
        _exit(42);
    }

    // Parent
    printf("parent PID=%ld, child PID=%ld\n", (long)getpid(), (long)pid);
    if (!immediate) {
        printf("sleeping %d seconds — check 'ps -l' in another terminal.\n",
               sleep_secs);
        sleep(sleep_secs);
    }

    int status = 0;
    pid_t w = waitpid(pid, &status, 0); // reap
    if (w < 0) { perror("waitpid"); return 1; }

    if (WIFEXITED(status)) {
        printf("reaped child %ld (exit=%d)\n", (long)w, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("reaped child %ld (signal=%d)\n", (long)w, WTERMSIG(status));
    } else {
        printf("reaped child %ld (other status)\n", (long)w);
    }
    return 0;
}
