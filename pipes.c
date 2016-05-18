#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "sys/wait.h"

int main(int argc, char* argv[]) {
    int input = 0;
    int output = 1;
    int status;
    for (int i = 1; i < argc; ++i) {
        int fildes[2];
        if (i + 1 == argc) {
            output = 1;
        } else {
            status = pipe(fildes);
            if (status == -1) {
                perror("");
                exit(-1);
            }
            output = fildes[1];
        }
        status = fork();
        if (status == -1) {
            perror("");
            exit(-1);
        }
        if (status == 0) {
            if (i + 1 != argc) {
                close(fildes[0]);
            }
            if (input != 0) {
                close(0);
                dup2(input, 0);
                close(input);
            }
            if (output != 1) {
                close(1);
                dup2(output, 1);
                close(output);
            }
            status = execlp(argv[i], argv[i], NULL);
            if (status == -1) {
                perror("");
                exit(-1);
            }
            exit(1);
        }
        close(input);
        close(output);
        input = fildes[0];
    }
    for (int i = 1; i < argc; ++i) {
        wait(NULL);
    }
}
