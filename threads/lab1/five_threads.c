#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int GLOBAL = 5;

void *mythread(void *arg) {
    int local = 10;
    static int local_static = 15;
    const int local_const = 20;
    printf("mythread [%d %d %d %lu]: Hello from mythread! glob: %p loc: %p stat: %p const:%p\n", getpid(), getppid(), gettid(), pthread_self(), &GLOBAL, &local, &local_static, &local_const);
    /*printf("--%d %d--\n", GLOBAL, local);
    GLOBAL = 6;
    local = 11;*/
    return NULL;
}

int main() {
    pthread_t tid[5];
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    for(int i = 0; i < 5; ++i) {
        err = pthread_create(&tid[i], NULL, mythread, NULL);
        //printf("%lu\n", tid[i]);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }
    sleep(60);
    for(int i = 0; i < 5; ++i) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}
