#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    while (1) {
        printf("i'm working\n");
        sleep(1000);
    }
    return NULL;
}

int main() {
    pthread_t tid;
    int err;
    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    sleep(3);
    if(pthread_cancel(tid) != 0) {
        printf("error in pthread cancel");
    }
    pthread_join(tid, NULL);
    return 0;
}
