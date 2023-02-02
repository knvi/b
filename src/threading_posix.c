#include <assert.h>
#ifndef _WIN32
#include <pthread.h>
#include <stdlib.h>
#include "threading.h"
static pthread_t *threads = NULL;
static int n_threads = 0;
void *start_thread(ThreadHandler fn, void *data) {
    n_threads++;
    if(!threads) {
        threads = malloc(n_threads * sizeof(pthread_t));
    } else {
        threads = realloc(threads, n_threads * sizeof(pthread_t));
    }
    assert(threads);
    pthread_t *thread_ptr = threads + (n_threads - 1);
    pthread_create(thread_ptr, NULL, fn, data);
    return thread_ptr;
}

void close_threads() {
    for(register int i = 0; i < n_threads; i++) {
        pthread_t *thread = threads + i;
        pthread_join(*thread, NULL);
    }
    free(threads);
}
#endif