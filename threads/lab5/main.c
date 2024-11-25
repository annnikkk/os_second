#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

volatile int done = 0;

void sigint_handler() {
    const char *message = "SIGINT\n";
    write(STDOUT_FILENO, message, 6);
}

void *mythread1(void *arg) {
    printf("thread 1 is working\n");

    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    printf("thread 1 blocked all signals\n");

    while (!done) {
        sleep(1);
    }

    pthread_exit(NULL);
}

void *mythread2(void *arg) {
    printf("thread 2 is working\n");

    signal(SIGINT, sigint_handler);

    printf("thread 2 waiting for sigint\n");

    while (!done) {
        sleep(1);
    }

    pthread_exit(NULL);
}
void *mythread3(void *arg) {
    printf("thread 3 is working\n");

    sigset_t set;
    int sig;

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    printf("waiting for SIGQUIT\n");
    sigwait(&set, &sig);
    pthread_exit(NULL);
}


int main() {
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    pthread_create(&tid1, NULL, mythread1, NULL);
    pthread_create(&tid2, NULL, mythread2, NULL);
    pthread_create(&tid3, NULL, mythread3, NULL);

    sleep(50);
    done = 1;
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    return 0;
}
