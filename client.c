#include "sys/socket.h"
#include "netinet/in.h"
#include "sys/un.h"
#include <stdio.h>

int s, d;

int main() {
    s = socket(PF_LOCAL, SOCK_STREAM, IPPROTO_IP);
    perror("");
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, "/tmp/test_socket");
    perror("");
    listen(s, 10);
    perror("");
    connect(s, (struct sockaddr*)&addr, sizeof(addr));
    perror("");
    send(s, "hello", 5, 0);
    close(s);
}
