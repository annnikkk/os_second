#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) {
    queue_t *q = (queue_t *) arg;

    printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        queue_print_stats(q);
        sleep(1);
    }

    return NULL;
}

queue_t *queue_init(int max_count) {
    int err;

    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    q->first = NULL;
    q->last = NULL;
    q->max_count = max_count;
    q->count = 0;

    //pthread_spin_init(&q->spinlock, PTHREAD_PROCESS_PRIVATE);
    //pthread_mutex_init(&q->mutex, PTHREAD_PROCESS_PRIVATE);
    sem_init(&q->semaphore, 0, 1);
    //pthread_cond_init(&q->not_empty, NULL);
    //pthread_cond_init(&q->not_full, NULL);

    q->add_attempts = q->get_attempts = 0;
    q->add_count = q->get_count = 0;

    err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
    if (err) {
        printf("queue_init: pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    return q;
}

void queue_destroy(queue_t *q) {
    while (q->count != 0) {
        qnode_t *tmp = q->first;
        q->first = q->first->next;
        free(tmp);
        q->count--;
    }

    //pthread_spin_destroy(&q->spinlock);
    //pthread_mutex_destroy(&q->mutex);
    sem_destroy(&q->semaphore);
    //pthread_cond_destroy(&q->not_empty);
    //pthread_cond_destroy(&q->not_full);

    pthread_cancel(q->qmonitor_tid);
    pthread_join(q->qmonitor_tid, NULL);
    free(q);
}

int queue_add(queue_t *q, int val) {
    q->add_attempts++;

    //pthread_spin_lock(&q->spinlock);
    //pthread_mutex_lock(&q->mutex);
    sem_wait(&q->semaphore);
    assert(q->count <= q->max_count);

    if (q->count == q->max_count) {
        //pthread_spin_unlock(&q->spinlock);
        //pthread_mutex_unlock(&q->mutex);
        sem_post(&q->semaphore);
        // while (q->count == q->max_count) {
        //     pthread_cond_wait(&q->not_full, &q->mutex);
        // }
        return 0;
    }
    //pthread_spin_unlock(&q->spinlock);
    //pthread_mutex_unlock(&q->mutex);
    sem_post(&q->semaphore);

    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new) {
        printf("Cannot allocate memory for new node\n");
        abort();
    }
    new->val = val;
    new->next = NULL;

    //pthread_spin_lock(&q->spinlock);
    //pthread_mutex_lock(&q->mutex);
    sem_wait(&q->semaphore);
    if (!q->first)
        q->first = q->last = new;
    else {
        q->last->next = new;
        q->last = q->last->next;
    }

    q->count++;
    q->add_count++;
    //pthread_spin_unlock(&q->spinlock);
    //pthread_mutex_unlock(&q->mutex);
    //pthread_cond_signal(&q->not_empty);
    sem_post(&q->semaphore);

    return 1;
}

int queue_get(queue_t *q, int *val) {
    q->get_attempts++;

    // pthread_spin_lock(&q->spinlock);
    //pthread_mutex_lock(&q->mutex);
    sem_wait(&q->semaphore);
    assert(q->count >= 0);

    if (q->count == 0) {
        //pthread_spin_unlock(&q->spinlock);
        //pthread_mutex_unlock(&q->mutex);
        sem_post(&q->semaphore);
        // while (q->count == 0) {
        //     pthread_cond_wait(&q->not_empty, &q->mutex);
        // }
        return 0;
    }

    qnode_t *tmp = q->first;

    if (q->first != NULL) {
        *val = tmp->val;
        q->first = q->first->next;
    }

    free(tmp);
    q->count--;
    //pthread_spin_unlock(&q->spinlock);
    //pthread_mutex_unlock(&q->mutex);
    //pthread_cond_signal(&q->not_full);
    sem_post(&q->semaphore);
    q->get_count++;

    return 1;
}


void queue_print_stats(queue_t *q) {
    //pthread_spin_lock(&q->spinlock);
    //pthread_mutex_lock(&q->mutex);
    sem_wait(&q->semaphore);
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
           q->count,
           q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
           q->add_count, q->get_count, q->add_count - q->get_count);
    //pthread_spin_unlock(&q->spinlock);
    //pthread_mutex_unlock(&q->mutex);
    sem_post(&q->semaphore);
}
