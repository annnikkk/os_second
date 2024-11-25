#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct myStruct {
    int field;
};

void myFunc(void* arg) {
    struct myStruct* myArg = (struct myStruct*)arg;
    printf("%d ", myArg->field);
}

int main() {
    struct myStruct example;
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    example.field = 55;
    int err = pthread_create(&tid, &attr, myFunc, &example);
    if (err) {
        printf("pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    pthread_exit(NULL);
    //pthread_join(tid, NULL);
}