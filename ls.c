#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

const char* MOD = "rwx";

size_t ls(const char* dir_name) {
    DIR* d;
    size_t size = 0;
    printf("%s:\n", dir_name);
    d = opendir(dir_name);
    if (!d) {
        fprintf(stderr, "Cannot open directory '%s'", dir_name);
        return 0;
    }
    while (1) {
        struct dirent* entry;
        const char* entry_name;
        entry = readdir(d);
        if (!entry) {
            break;
        }
        entry_name = entry->d_name;
        if (entry_name[0] == '.') {
            continue;
        }
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", dir_name, entry_name);
        struct stat sb;
        stat(path, &sb);
        switch (sb.st_mode & S_IFMT) {
            case S_IFDIR:  printf("d");
                           break;
            case S_IFLNK:  printf("l");
                           break;
            case S_IFREG:  printf("-");
                           break;
            default:       printf("?"); 
                           break;
        }
        for (int i = 0; i < 9; ++i) {
            putchar(sb.st_mode & (1 << (8 - i)) ? MOD[i % 3] : '-');
        }
        printf(" %li %s %s %s\n", sb.st_size, (getpwuid(sb.st_uid))->pw_name, (getgrgid(sb.st_gid))->gr_name, entry_name);
        size += sb.st_size;
        if (entry->d_type & DT_DIR) {
            size += ls(path);
        }
    }
    closedir(d);
    printf("total %lu\n\n", size);
    return size;
}

int main(int argc, char* argv[]) {
    const char* name = ".";
    if (argc > 1) {
        name = argv[1];
    }
    ls(name);
}



