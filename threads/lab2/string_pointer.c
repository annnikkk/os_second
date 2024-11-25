#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void* mythread(void *arg) {
    char* res = malloc(sizeof(char) * 12);
    strcpy(res, "hello world");
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
    return res;
}

int main() {
    pthread_t tid;
    int err;
    char* res;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_join(tid,(void**) &res);
    printf("%s", res);
    free(res);
    return 0;
}
