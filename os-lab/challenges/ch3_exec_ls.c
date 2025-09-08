#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();
    
    if (pid == -1) {
        // Fork failed
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0) {
        // Child process
        execl("/bin/ls", "ls", "-la", NULL);
        // If execl returns, it means it failed
        perror("execl failed");
        exit(1);
    }
    else {
        // Parent process
        int status;
        wait(&status);  // Wait for child to finish
        printf("Child process completed.\n");
    }
    
    return 0;
}