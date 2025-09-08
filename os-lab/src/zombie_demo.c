// src/pipeline_demo.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static void die(const char *msg) { perror(msg); exit(1); }

int main(int argc, char *argv[]) {
    const char *pattern = (argc > 1) ? argv[1] : ".";  // default: match all

    int fd[2];
    if (pipe(fd) == -1) die("pipe");

    /* ---------- child #1: runs `ls -1`, stdout -> pipe write end ---------- */
    pid_t c1 = fork();
    if (c1 < 0) die("fork c1");

    if (c1 == 0) {
        // writer child
        if (dup2(fd[1], STDOUT_FILENO) == -1) die("dup2 ls->stdout");
        close(fd[0]);          // not reading
        close(fd[1]);          // stdout now duplicated

        execlp("ls", "ls", "-1", (char *)NULL);
        die("execlp ls");
    }

    /* ---------- child #2: runs `grep <pattern>`, stdin <- pipe read end --- */
    pid_t c2 = fork();
    if (c2 < 0) die("fork c2");

    if (c2 == 0) {
        // reader child
        if (dup2(fd[0], STDIN_FILENO) == -1) die("dup2 grep->stdin");
        close(fd[1]);          // not writing
        close(fd[0]);          // stdin now duplicated

        execlp("grep", "grep", pattern, (char *)NULL);
        die("execlp grep");
    }

    /* ---------- parent: close both ends, wait for both children ----------- */
    close(fd[0]);
    close(fd[1]);

    int status;
    waitpid(c1, &status, 0);
    waitpid(c2, &status, 0);

    return 0;
}
