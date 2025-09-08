// ch7_pipeline.c
// Usage: ./ch7_pipeline "<regex pattern>"
// Implements: ls | grep <pattern>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s \"<pattern>\"\n", argv[0]);
        return 1;
    }
    char *pattern = argv[1];

    int pf[2];
    if (pipe(pf) == -1) { perror("pipe"); return 1; }

    // ---- First child: runs `ls`, stdout -> pipe write end ----
    pid_t c1 = fork();
    if (c1 < 0) { perror("fork"); return 1; }
    if (c1 == 0) {
        // child A
        close(pf[0]);                // close read end
        if (dup2(pf[1], STDOUT_FILENO) == -1) { perror("dup2 ls"); _exit(1); }
        close(pf[1]);                // not needed after dup2
        execlp("ls", "ls", (char*)NULL);
        perror("execlp ls");
        _exit(1);
    }

    // ---- Second child: runs `grep <pattern>`, stdin <- pipe read end ----
    pid_t c2 = fork();
    if (c2 < 0) { perror("fork"); return 1; }
    if (c2 == 0) {
        // child B
        close(pf[1]);                // close write end
        if (dup2(pf[0], STDIN_FILENO) == -1) { perror("dup2 grep"); _exit(1); }
        close(pf[0]);                // not needed after dup2
        execlp("grep", "grep", pattern, (char*)NULL);
        perror("execlp grep");
        _exit(1);
    }

    // ---- Parent: close both ends, wait for children ----
    close(pf[0]);
    close(pf[1]);
    int st;
    waitpid(c1, &st, 0);
    waitpid(c2, &st, 0);
    return 0;
}
