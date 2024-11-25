#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
int thread_count = 0;

void *mythread(void *arg) {
    pthread_mutex_lock(&count_mutex);
    thread_count ++;
    pthread_mutex_unlock(&count_mutex);
    printf("Thread number: %d, Thread ID: %lu\n", thread_count, pthread_self());
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    printf("pid: %d\n", getpid());
    sleep(20);
    while (1) {
        if (pthread_create(&thread, &attr, mythread, NULL) != 0) {
            usleep(1000);
            pthread_mutex_lock(&count_mutex);
            printf("Failed to create thread after creating %d threads\n", thread_count);
            pthread_mutex_unlock(&count_mutex);
            return 1;
        }
        usleep(1000);
        if(thread_count == 1 || thread_count == 2 || thread_count == 3) {
            printf("i'm sleeping\n");
            sleep(10);
        }
    }

    pthread_mutex_destroy(&count_mutex);
    return 0;
}
