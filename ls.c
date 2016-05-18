#define _DEFAULT_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/limits.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

const char* MOD = "rwx";

size_t ls(const char* dir_name) {
    DIR* d;
    printf("%s:\n", dir_name);
    d = opendir(dir_name);
    if (!d) {
        fprintf(stderr, "Cannot open directory '%s'", dir_name);
        return 0;
    }
    long begining = telldir(d);
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
        lstat(path, &sb);
        if (S_ISDIR(sb.st_mode)) {
            printf("d");
        } else if (S_ISLNK(sb.st_mode)) {
            printf("l");
        } else if (S_ISREG(sb.st_mode)) {
            printf("-");
        } else {
            printf("?");
        }
        for (int i = 0; i < 9; ++i) {
            putchar(sb.st_mode & (1 << (8 - i)) ? MOD[i % 3] : '-');
        }
        struct passwd* pw_struct = getpwuid(sb.st_uid);
        struct group* gr_struct = getgrgid(sb.st_gid);
        char* user_name;
        char* group_name;
        if (pw_struct) {
            user_name = pw_struct->pw_name;
        } else {
            user_name = malloc(40);
            sprintf(user_name, "%d", sb.st_uid);
        }
        if (gr_struct) {
            group_name = gr_struct->gr_name;
        } else {
            group_name = malloc(40);
            sprintf(group_name, "%d", sb.st_gid);
        }
        char* date = malloc(40);
        strftime(date, 20, "%d-%m-%y", localtime(&(sb.st_mtime)));
        printf(" %li %s %s %li %s %s", sb.st_nlink, user_name, group_name, sb.st_size, date, entry_name);
        if (S_ISLNK(sb.st_mode)) {
            char* linkname = malloc(sb.st_size + 1);
            linkname = malloc(sb.st_size + 1);
            int r = readlink(path, linkname, sb.st_size + 1);
            if (r == -1) {
                perror("readlink");
                exit(EXIT_FAILURE);
            }
            linkname[r] = '\0';
            printf(" -> %s", linkname);
        }
        printf("\n");
    }
    printf("\n");
    seekdir(d, begining);
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
        if (S_ISDIR(sb.st_mode)) {
            ls(path);
        }
    }
    closedir(d);
    return 0;
}

int main(int argc, char* argv[]) {
    const char* name = ".";
    if (argc > 1) {
        name = argv[1];
    }
    ls(name);
}
