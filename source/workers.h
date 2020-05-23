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
#include <stdbool.h>
#include <openssl/sha.h>

#include "common.h"

#define BLOCKSIZE 15
#define QUEUE_MAXMSG 20
#define QUEUE_MSGSIZE 18
#define QUEUE_PERMS ((int)(0644))
#define MAX_WORKERS 2
#define QUEUE_ATTR_INITIALIZER ((struct mq_attr){0, QUEUE_MAXMSG, QUEUE_MSGSIZE, 0})

typedef unsigned int WorkerId;
typedef unsigned int WorkUnitId;
typedef void (*ProcFunc_t)(void *ctx);

typedef struct __attribute__((__packed__)) WorkUnit
{
    WorkUnitId id;
    ProcFunc_t fun;
    void *context;
} WorkUnit_t;

typedef struct Worker 
{

    WorkerId id; //los identifico porque los quiero
    int processing;
    int32_t goldNonce;
    mqd_t reqQueue;
    // la direccion donde escriben
    int32_t *minerNonce;
    pthread_t thread_load;

} Worker_t;

typedef struct Context 
{
    
    char block[BLOCKSIZE];
    int32_t nonce;
    int32_t section;
    int difficulty;
    bool found;

} Context_t;

Worker_t * workerInit( int workerQty, int32_t * pGoldNonce, char *minerQueue);

// comienzo a procesar los threads de la cola
void workerRun(Worker_t *workers, int workerQty);

void workerStop(Worker_t *workers, int workerQty);

void workerWait(Worker_t *workers, int workerQty);
// hago laburar a uno de la lista de cosas que hay que hacer
void *thread_run(void *ctx);

// void workerCheck(Worker_t *workers, int workerQty);

void workerDestroy(Worker_t *workers);

//funciones del worker
void hashBlock(void *ctx);

#endif