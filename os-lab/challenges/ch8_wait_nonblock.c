// ch8_wait_nonblock.c
// Spawn 3 children that sleep 1, 2, 3 seconds, respectively.
// Parent polls with waitpid(-1, &status, WNOHANG) and prints completions
// as soon as they happen (no blocking on other children).

#define _XOPEN_SOURCE 700
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static pid_t spawn_sleeper(int secs) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        // child
        sleep(secs);
        _exit(secs); // return sleep seconds as exit status (0..255)
    }
    return pid; // parent
}

int main(void) {
    int sleeps[3] = {1, 2, 3};
    pid_t kids[3];

    for (int i = 0; i < 3; ++i) {
        kids[i] = spawn_sleeper(sleeps[i]);
        printf("spawned child %d (PID %ld) sleeping %d s\n",
               i, (long)kids[i], sleeps[i]);
    }

    int remaining = 3;
    while (remaining > 0) {
        int status;
        pid_t done = waitpid(-1, &status, WNOHANG); // non-blocking
        if (done > 0) {
            // One child finished
            int secs = -1;
            for (int i = 0; i < 3; ++i) {
                if (kids[i] == done) { secs = sleeps[i]; break; }
            }

            if (WIFEXITED(status)) {
                int code = WEXITSTATUS(status);
                printf("child PID %ld finished (slept ~%d s, exit=%d)\n",
                       (long)done, secs, code);
            } else if (WIFSIGNALED(status)) {
                printf("child PID %ld terminated by signal %d\n",
                       (long)done, WTERMSIG(status));
            } else {
                printf("child PID %ld finished (non-standard status)\n",
                       (long)done);
            }
            --remaining;
        } else if (done == 0) {
            // No child finished yet — do not block; small nap to avoid busy spin
            usleep(50 * 1000); // 50 ms
        } else { // done < 0
            if (errno == EINTR) continue; // interrupted by signal — retry
            if (errno == ECHILD) break;   // no more children
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }

    puts("parent: all children have finished.");
    return 0;
}
