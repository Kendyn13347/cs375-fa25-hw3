// ch4_exec_worker.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // Child: exec worker with args and custom environment
        char *const argv[] = { "./worker", "arg1", "arg2", "arg3", NULL };
        char *const envp[] = { "MYVAR=hello", NULL };

        // Use execve to pass an explicit environment
        execve("./worker", argv, envp);

        // If execve returns, it failed:
        perror("execve worker");
        _exit(127);
    }

    // Parent: wait and report status
    int status = 0;
    if (waitpid(pid, &status, 0) == -1) { perror("waitpid"); return 1; }

    if (WIFEXITED(status)) {
        printf("[parent] worker exited with status %d\n", WEXITSTATUS(status));
    } else {
        printf("[parent] worker did not exit normally\n");
    }
    return 0;
}

