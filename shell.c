#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int pd[2];
    pipe(pd);
    if (fork()) {
        dup2(pd[1], 1);
        close(pd[0]);
        close(pd[1]);
        execlp("ls", "ls", NULL);
    } else if (fork()) {
        dup2(pd[0], 0);
        close(pd[0]);
        close(pd[1]);
        execlp("wc", "wc", NULL);
    }
    close(pd[0]);
    close(pd[1]);
    int status;
    for (int i = 9; i < 2; ++i) {
        wait(&status);
    }
}
