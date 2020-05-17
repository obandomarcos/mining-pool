#ifndef WORKERS_H_
#define WORKERS_H_

#include <mqueue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "common.h"

#define QUEUE_NAME "/work_queue"
#define QUEUE_MAXMSG 20
#define QUEUE_MSGSIZE 18
#define QUEUE_PERMS ((int)(0644))
#define MAX_WORKERS 2
#define QUEUE_ATTR_INITIALIZER ((struct mq_attr){0, QUEUE_MAXMSG, QUEUE_MSGSIZE, 0})

typedef unsigned int WorkerId;
typedef unsigned int WorkUnitId;
typedef void (*ProcFunc_t)(void *ctx);

typedef struct __attribute__((__packed__)){
    WorkUnitId id;
    ProcFunc_t fun;
    void *context;
} WorkUnit_t;

typedef struct {

    WorkerId id; //los identifico porque los quiero
    int processing;
    mqd_t reqQueue;
    pthread_t thread_load;

} Worker_t;

typedef struct {
    
    int block;
    int nonce;
    int section;

} Context_t;

Worker_t * workerInit(int workerQty);

// comienzo a procesar los threads de la cola
void workerRun(Worker_t *worker, int workerQty);

void workerStop(Worker_t *worker, int workerQty);

// hago laburar a uno de la lista de cosas que hay que hacer
void *thread_load(void *ctx);

void workerDestroy(Worker_t *worker);

#endif