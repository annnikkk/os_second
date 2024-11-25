#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    int* res = malloc(sizeof(int));
    *res = 42;
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
    return (void*) res;
}

int main() {
    pthread_t tid;
    int err;
    int* res;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_join(tid,(void**) &res);
    printf("pthread_join() returned %d\n", *res);
    return 0;
}
