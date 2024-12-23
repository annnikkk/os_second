#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    //pthread_detach(pthread_self());
    printf("mythread [%d %d %d %lu]: Hello from mythread!\n", getpid(), getppid(), gettid(), pthread_self());
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_attr_t attr;
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    //pthread_attr_init(&attr);
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    while (1) {
        err = pthread_create(&tid, NULL, mythread, NULL);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
        //sleep(5);
        //pthread_join(tid, NULL);
    }
    return 0;
}
