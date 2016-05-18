#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

int main()
{

    char str[100];
    int listen_fd, comm_fd;
    struct sockaddr_in servaddr;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(5757);
    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(listen_fd, 10);
    while (1) {
        comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
        bzero(str, 100);
        read(comm_fd, str, 100);
        int status = fork();
        if (status == 0) {
            close(1);
            dup2(comm_fd, 1);
            int pos = 0;
            char* args[1000];
            args[0] = str;
            int argc = 1;
            while (1) {
                while (str[pos] != '\n' && str[pos] != ' ') {
                    ++pos;
                }
                if (str[pos] == '\n') {
                    str[pos] = '\0';
                    break;
                }
                str[pos] = '\0';
                ++pos;
                args[argc] = str + pos;
                ++argc;
            }
            args[argc] = NULL;
            execvp(str, args);
            perror("execlp: ");
        } else {
            close(comm_fd);
        }
    }
}

