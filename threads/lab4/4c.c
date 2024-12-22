#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    char* str = malloc(sizeof(char) * 12);
    strcpy(str, "hello world");
    pthread_cleanup_push(free, str);
    while (1) {
        printf("%s\n", str);
    }
    pthread_cleanup_pop(1);
    return NULL;
}
/*
do {
    __pthread_unwind_buf_t __cancel_buf;
    void (*__cancel_routine)(void *) = (free);
    void *__cancel_arg = (str);
    int __not_first_call = __sigsetjmp_cancel(__cancel_buf.__cancel_jmp_buf, 0); есть еще варианты сюда попасть
    if (__builtin_expect((__not_first_call), 0)) {
        __cancel_routine(__cancel_arg);
        __pthread_unwind_next(&__cancel_buf);
    }
    __pthread_register_cancel(&__cancel_buf);
    do {
        while (1) {
            printf("%s\n", str);
        }
        do {
        } while (0);
    } while (0);
    __pthread_unregister_cancel(&__cancel_buf);
    if (1) __cancel_routine(__cancel_arg);
} while (0);
*/

void cleanup_handler(void *arg) {
    char* str = (char*) arg;
    if(str) {
        pthread_cleanup_push(cleanup_handler, str);
        //pthread_exit(0);
        printf("чистим %s", str);
        pthread_cleanup_pop(1);
    }
}

void *mythread2(void *arg) {
    char* str = malloc(sizeof(char) * 12);
    strcpy(str, "hello world");
    pthread_cleanup_push(cleanup_handler, str);
    for(int i = 0; i < 3; i++) {
        printf("%s\n", str);
        sleep(1);
        //pthread_exit(0);
    }
    pthread_cleanup_pop(1);
    free(str);
    return NULL;
}

int main() {
    pthread_t tid;
    int err;
    err = pthread_create(&tid, NULL, mythread2, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    sleep(2);
    /*if(pthread_cancel(tid) != 0) {
        printf("error in pthread cancel");
    }*/
    pthread_join(tid, NULL);

    return 0;
}
