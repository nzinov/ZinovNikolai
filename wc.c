#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int total_lines = 0;
int total_words = 0;
int total_chars = 0;
int used_stdin = 0;

void process(const char* fname) {
    FILE* file = stdin;
    if (fname && fname[0] != '-') {
        file = fopen(fname, "r");
        if (!file) {
            fprintf(stderr, "No such file '%s'", fname);
            return;
        }
    } else {
        if (used_stdin) {
            fprintf(stderr, "Stdin can be used only once");
            return;
        }
        used_stdin = 1;
    }
    int current_char = 0;
    int line_number = 0;
    int word_number = 0;
    int char_number = 0;
    int word_started = 0;
    while ((current_char = getc(file)) != -1) {
        ++char_number;
        if (current_char == '\n') {
            ++line_number;
        }
        if (isspace(current_char)) {
            if (word_started) {
                ++word_number;
                word_started = 0;
            }
        } else {
            word_started = 1;
        }
    }
    if (fname) {
        printf("%d\t%d\t%d\t%s\n", line_number, word_number, char_number, fname);
    } else {
        printf("%d\t%d\t%d\n", line_number, word_number, char_number);
    }
    total_chars += char_number;
    total_lines += line_number;
    total_words += word_number;
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        process(argv[i]);
    }
    if (argc > 2) {
        printf("%d\t%d\t%d\ttotal\n", total_lines, total_words, total_chars);
    } else if (argc == 1) {
        process(0);
    }
}

