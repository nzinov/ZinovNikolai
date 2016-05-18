#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define _BSD_SOURCE

const int MAX = 2 << 16;
FILE* output;

void dump_size(size_t size) {
    if (size >= MAX) {
        fprintf(stderr, "Too long");
        exit(-1);
    }
    short small_len = size;
    fwrite(&small_len, 2, 1, output);
}

void sdump(char* str) {
    size_t len = strlen(str);
    dump_size(len);
    fputs(str, output);
}

void dump(char* fname) {
    DIR* dir = opendir(fname);
    if (!dir) {
        perror(fname);
        exit(-1);
    }
    int has_files = 0;
    while (1) {
        struct dirent* entry;
        entry = readdir(dir);
        if (!entry) {
            break;
        }
	if (entry->d_name[0] == '.') {
	    continue;
	}
        has_files = 1;
        char* next = malloc(1024);
        sprintf(next, "%s/%s", fname, entry->d_name);
        switch (entry->d_type) {
            case DT_DIR:
                dump(next);
                break;
            case DT_LNK:
                sdump(next);
                sdump("");
                char* comment = malloc(1024);
                char* lname = malloc(1024);
                readlink(next, lname, 1024);
                sprintf(comment, "link to: %s", lname);
                sdump(comment);
                free(comment);
                free(lname);
                break;
            case DT_REG:
                sdump(next);
                FILE* f = fopen(next, "r");
                size_t buff_size = 1024;
                char* buffer = malloc(buff_size);
                size_t read_count = 0;
                while (1) {
                    if (read_count + 1024 >= buff_size) {
                        buff_size *= 2;
                        buffer = realloc(buffer, buff_size);
                    }
                    size_t current = fread(buffer, 1, 1024, f);
                    if (current == 0) {
                        break;
                    }
                    read_count += current;
                }
                dump_size(read_count);
                fwrite(buffer, read_count, 1, output);
                free(buffer);
                struct stat sb;
                if (stat(next, &sb) == -1) {
                    perror("");
                    exit(-1);
                }
                char* comment2 = malloc(1024);
                sprintf(comment2, "uid=%d", sb.st_uid);
                sdump(comment2);
                free(comment2);
                break;
        }
        free(next);
    }
    if (!has_files) {
        char* next = malloc(1024);
        sprintf(next, "%s/", fname);
	sdump(next);
        sdump("");
        sdump("empty");
        free(next);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <directory> <target>", argv[0]);
        exit(-1);
    }
    output = fopen(argv[2], "w");
    dump(argv[1]);
}
