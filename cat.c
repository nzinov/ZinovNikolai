#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int BUFF_SIZE = 1024;

int main(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "u")) != -1) {
        switch(opt) {
            case 'u':
                BUFF_SIZE = 1;
                break;
        }
    }
    int used_stdin = 0;
    for (; optind < argc; ++optind) {
        int current_file = STDIN_FILENO;
        if (argv[optind][0] == '-') {
            if (used_stdin) {
                continue;
            } else {
                used_stdin = 1;
            }
        } else {
            current_file = open(argv[optind], O_RDONLY);
        }
        char* buf = (char*)malloc(BUFF_SIZE);
        ssize_t read_bytes;
        while ((read_bytes = read(current_file, buf, BUFF_SIZE)) > 0) {
            write(STDOUT_FILENO, buf, read_bytes);
        }
        close(current_file);
    }
}

