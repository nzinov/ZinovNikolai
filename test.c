#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    int k;
    k = fork();
    if (k < 0)  {
        perror("_");
        exit(1);
    } else if (k > 0) {
        sleep(1);
        printf("Parent pid=%d ppid=%d\n", getpid(), getppid());
        kill(k, 9);
        sleep(100);
    } else {
        printf("Child pid=%d ppid=%d\n", getpid(), getppid());
        kill(getppid(), 9);
        exit(0);
    }
}

