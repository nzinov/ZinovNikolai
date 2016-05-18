#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>


typedef unsigned long long ull;

char* buf;
ull max = -1;
const size_t CHUNK_SIZE = 1024*1024;
size_t chunk = 0;
size_t thread_num = 4;

void* sieve(void* shift) {
    for (ull i = 2 + (size_t)shift; i*i < (chunk + 1)*CHUNK_SIZE; i += thread_num) {
        for (ull pos = 2*i; pos < max && pos < (chunk + 1)*CHUNK_SIZE; pos += i) {
            if (pos < chunk*CHUNK_SIZE) {
                continue;
            }
            buf[pos - chunk*CHUNK_SIZE] = 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        thread_num = atoi(argv[1]);
    }
    if (argc > 2) {
        max = strtoull(argv[2], NULL, 10);
    }
    pthread_t* tid = malloc(thread_num * sizeof(pthread_t));
    for (chunk = 0; chunk == 0 || (chunk - 1)*CHUNK_SIZE < max; ++chunk) {
        buf = malloc(CHUNK_SIZE);
        bzero(buf, CHUNK_SIZE);
        for (size_t i = 0; i < thread_num; ++i) {
            pthread_create(&tid[i], NULL, sieve, (void*)i);
        }
        for (size_t i = 0; i < thread_num; ++i) {
            pthread_join(tid[i], NULL);
        }
        for (size_t pos = 0; pos < CHUNK_SIZE; ++pos) {
            if (chunk == 0 && pos < 2) {
                continue;
            }
            if (pos + chunk*CHUNK_SIZE >= max) {
                break;
            }
            if (buf[pos] == 0) {
                printf("%lu ", pos + chunk*CHUNK_SIZE);
            }
        }
        free(buf);
    }
}
