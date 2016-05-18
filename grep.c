#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

#define KNRM  "\x1B[0m"
#define KGREY "\x1B[37m"
#define KRED  "\x1B[31m"

int match(const char* tmplt, const char* text, size_t tpos, size_t pos) {
    if (!tmplt[tpos]) {
        return 0;
    }
    if (!text[pos]) {
        return -1;
    }
    char next = text[pos];
    char modif = '\0';
    int matched = 0;
    size_t i;
    size_t modif_pos;
    if (tmplt[tpos] == '[') {
        for (i = tpos + 1; tmplt[i] != ']'; ++i) {
            if (!tmplt[i]) {
                fprintf(stderr, "Unmatched '[' in template\n");
                exit(1);
            }
            if (tmplt[i] == '-' && tmplt[i-1] != '[' && tmplt[i+1] != ']') {
                if (tmplt[i-1] >= tmplt[i+1]) {
                    fprintf(stderr, "Bad range\n");
                    exit(1);
                }
                if (tmplt[i-1] < next && tmplt[i+1] > next) {
                    matched = 1;
                }
            } else if (tmplt[i] == next) {
                matched = 1;
            }
        }
        modif_pos = i+1;
    } else {
        matched = tmplt[tpos] == next || tmplt[tpos] == '.';
        modif_pos = tpos + 1;
    }
    modif = tmplt[modif_pos];
    size_t next_pos = modif_pos + (modif == '*' || modif == '?');
    if (matched) {
        int count = -1;
        if (modif == '*') {
            count =  match(tmplt, text, tpos, pos + 1);
            if (count == -1) {
                count = match(tmplt, text, next_pos, pos + 1);
            }
        } else {
            count = match(tmplt, text, next_pos, pos + 1);
        }
        if (count != -1) {
            count++;
        }
        return count;
    } else {
        if (modif == '*' || modif == '?') {
            return match(tmplt, text, next_pos, pos);
        } else {
            return -1;
        }
    }
}

void grepline(const char* tmplt, const char* text, const char* fname, bool print_fname) {
    bool matched = false;
    int last_match = 0;
    int i;
    for (i = 0; text[i] != '\0'; ++i) {
        int len;
        if ((len = match(tmplt, text, 0, i)) != -1) {
            if (!matched && print_fname) {
                printf(KGREY "%s: " KNRM, fname);
            }
            fwrite(text + last_match, 1, i - last_match, stdout);
            printf(KRED);
            fwrite(text + i, 1, len, stdout);
            printf(KNRM);
            last_match = i + len;
            matched = true;
            i = last_match;
        }
    }
    if (matched) {
        fwrite(text + last_match, 1, i - last_match, stdout);
        printf("\n");
    }
}

void grep(const char* tmplt, const char* fname, bool print_fname) {
    FILE* file;
    if (!fname) {
        file = stdin;
    } else {
        file = fopen(fname, "r");
    }
    char* buf;
    size_t len = 0;
    int read;
    while ((read = getline(&buf, &len, file)) != -1) {
        buf[read-1] = '\0';
        grepline(tmplt, buf, fname, print_fname);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s template [files]\n", argv[0]);
        exit(1);
    }
    const char* tmplt = argv[1];
    for (int i = 2; i < argc; ++i) {
        grep(tmplt, argv[i], argc > 3);
    }
    if (argc < 3) {
        grep(tmplt, 0, false);
    }
}
