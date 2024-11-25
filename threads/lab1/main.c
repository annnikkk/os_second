#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int main();

void *mythread(void *arg) {
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
    main();
    return NULL;
}

int main() {
    static int flag = 0;
    if (0 == flag) {
        flag = 1;
        pthread_t tid;
        int err;

        printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

        err = pthread_create(&tid, NULL, mythread, NULL);
        if (err) {
            return -1;
        }
        sleep(100);
       // pthread_exit(NULL);
    }
    else {
        printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());
        return 0;
    }

    //pthread_join(tid, NULL);
    return 0;
}
