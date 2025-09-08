// ch5_exec_examples.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static void wait_child(pid_t pid) {
    int st = 0;
    if (waitpid(pid, &st, 0) == -1) {
        perror("waitpid");
        exit(1);
    }
    if (WIFEXITED(st)) {
        // optional: report status
        // printf("[parent] child %d exited with %d\n", pid, WEXITSTATUS(st));
    }
}

int main(void) {
    pid_t pid;

    /* --- Child A: execl --- */
    pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        execl("/bin/echo", "echo", "one", "two", (char *)NULL);
        perror("execl");   // only if exec fails
        _exit(127);
    }
    wait_child(pid);

    /* --- Child B: execv --- */
    pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        char *argv[] = { "echo", "one", "two", NULL };
        execv("/bin/echo", argv);
        perror("execv");   // only if exec fails
        _exit(127);
    }
    wait_child(pid);

    return 0;
}
