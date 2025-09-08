// ch10_pool.c
// Usage: ./ch10_pool M task1 task2 ... taskN
// Spawns up to M children at a time. Each child "processes" its task,
// prints start/finish, and exits. Parent reaps and launches more.

#define _XOPEN_SOURCE 700
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    pid_t pid;
    const char *task;  // argv pointer (no copy needed)
} slot_t;

static void child_work(const char *task) {
    pid_t me = getpid();
    printf("[worker %ld] start: %s\n", (long)me, task);
    fflush(stdout);

    // Simulate variable work time (1–3s) from task name length.
    unsigned sec = (unsigned)(strlen(task) % 3) + 1;
    sleep(sec);

    printf("[worker %ld] finish: %s (took ~%us)\n", (long)me, task, sec);
    fflush(stdout);
    _exit(0);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <concurrency:M> <task1> [task2 ...]\n", argv[0]);
        return 1;
    }

    char *end = NULL;
    long M = strtol(argv[1], &end, 10);
    if (end == argv[1] || M <= 0) {
        fprintf(stderr, "Invalid concurrency: %s\n", argv[1]);
        return 1;
    }

    const int total = argc - 2;
    const char **tasks = (const char **)&argv[2];

    slot_t *slots = calloc((size_t)M, sizeof(slot_t));
    if (!slots) { perror("calloc"); return 1; }

    int next = 0;          // index of next task to start
    int done = 0;          // number of tasks completed
    int active = 0;        // number of running children

    while (done < total) {
        // Launch while we have capacity and tasks remaining
        while (active < M && next < total) {
            pid_t pid = fork();
            if (pid < 0) { perror("fork"); goto fail; }
            if (pid == 0) child_work(tasks[next]);

            // parent: record in a free slot
            for (long i = 0; i < M; ++i) {
                if (slots[i].pid == 0) { slots[i].pid = pid; slots[i].task = tasks[next]; break; }
            }
            ++active;
            ++next;
        }

        // Reap one finished child (blocking when at capacity / or nothing to launch)
        int status;
        pid_t donepid = waitpid(-1, &status, 0);
        if (donepid < 0) {
            if (errno == EINTR) continue;
            perror("waitpid");
            goto fail;
        }

        // Find which task this PID belonged to
        const char *tname = "?";
        for (long i = 0; i < M; ++i) {
            if (slots[i].pid == donepid) {
                tname = slots[i].task;
                slots[i].pid = 0;  // free slot
                break;
            }
        }

        if (WIFEXITED(status)) {
            printf("[parent] reaped %ld (task=%s, exit=%d)\n",
                   (long)donepid, tname, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[parent] reaped %ld (task=%s, signal=%d)\n",
                   (long)donepid, tname, WTERMSIG(status));
        } else {
            printf("[parent] reaped %ld (task=%s, other status)\n",
                   (long)donepid, tname);
        }
        fflush(stdout);

        --active;
        ++done;
    }

    free(slots);
    puts("[parent] all tasks complete.");
    return 0;

fail:
    free(slots);
    return 1;
}
