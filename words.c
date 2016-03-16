#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TRUE 1 
#define FALSE 0 

char* data;
size_t* words;
size_t word_count;
size_t reserved_words;
size_t data_length;
size_t reserved_length;
int in_word = FALSE;

void add_data(char c) {
    if (data_length == reserved_length) {
        reserved_length *= 2;
        data = (char*)realloc(data, sizeof(char)*reserved_length);
    }
    data[data_length] = c;
    ++data_length;
};

void start_word() {
    if (word_count == reserved_words) {
        reserved_words *= 2;
        words = (size_t*)realloc(words, sizeof(size_t*)*reserved_words);
    }
    words[word_count] = data_length;
    ++word_count;
    in_word = TRUE;
};

void finish_word() {
    if (in_word) {
        add_data('\0');
        in_word = FALSE;
    }
}

void add_word(char c) {
    start_word();
    add_data(c);
    finish_word();
}

int cmp(const void* a, const void* b) {
    return strcmp(data+*((size_t*)a), data+*((size_t*)b));
}

int main() {
    int next_char;
    data = (char*)malloc(sizeof(char)*10);
    words = (size_t*)malloc(sizeof(size_t)*10);
    word_count = 0;
    reserved_words = 10;
    data_length = 0;
    reserved_length = 10;
    char last_quote = '\0';
    char last_special_char = '\0';
    while ((next_char = getchar()) != EOF) {
        if (last_quote) {
            if (next_char == last_quote) {
                last_quote = '\0';
            } else {
                if (!in_word) {
                    start_word();
                }
                add_data(next_char);
            }
            continue;
        }
        if (last_special_char) {
            if (last_special_char != next_char) {
                finish_word();
                last_special_char = '\0';
            }
        }
        if (isspace(next_char)) {
            finish_word();
        } else {
            switch(next_char) {
                case '\'':
                case '\"':
                    last_quote = next_char;
                    break;
                case ';':
                    finish_word();
                    add_word(';');
                    break;
                case '|':
                case '&':
                    if (last_special_char != next_char) {
                        finish_word();
                        last_special_char = next_char;
                        start_word();
                        add_data(next_char);
                    } else {
                        add_data(next_char);
                        finish_word();
                        last_special_char = '\0';
                    }
                    break;
                default:
                    if (!in_word) {
                        start_word();
                    }
                    add_data(next_char);
            }
        }
    }
    finish_word();
    if (last_quote) {
        fputs("Unmatched quote found!", stderr);
        abort();
    }
    qsort(words, word_count, sizeof(char*), cmp);
    for (size_t i = 0; i < word_count; ++i) {
        putchar('"');
        fputs(data+words[i], stdout);
        putchar('"');
        putchar('\n');
    }
}
