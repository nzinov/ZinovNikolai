#include "sys/socket.h"
#include "netinet/in.h"
#include "sys/un.h"
#include <stdio.h>
#include "malloc.h"
#include <unistd.h>

int s, d;

int main() {
    s = socket(PF_LOCAL, SOCK_STREAM, IPPROTO_IP);
    perror("");
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, "/tmp/test_socket");
    bind(s, (struct sockaddr*)&addr, sizeof(addr));
    perror("");
    listen(s, 10);
    perror("");
    int d = accept(s, NULL, NULL);
    char* buf = (char*)malloc(5);
    recv(d, buf, 5, 0);
    write(1, buf, 5);
    perror("");
    shutdown(d);
    shutdown(s);
}
