#ifndef DATABASE_H_
#define DATABASE_H_

#define STORAGE_ID "/DATABASE_TESTING"
#define STORAGE_SIZE 1024
#define MAX_QUEUE_ELEMENTS 5

int proteccion = PROT_READ | PROT_WRITE;
int visibilidad = MAP_SHARED | MAP_ANONYMOUS;


typedef unsigned int MinerId;
typedef unsigned int uint;

// MinerReg_t allows to have a registry of miners connected to the pool
typedef struct {

    MinerId id;
    int wallet;
    struct sockaddr_in miner_addr;

} MinerReg_t;


typedef struct{
    
    sem_t sem_get;
    sem_t sem_put;
    sem_t sem_lock;

    unsigned int getter;
    unsigned int putter;
    unsigned int qSize;

    MinerReg_t miners[0];

} MinerDB_t;


MinerDB_t* DatabaseCreate();
void DatabaseDestroy(MinerDB_t *queue);

void DatabasePut(MinerDB_t *queue, MinerReg_t elem);
MinerReg_t DatabaseGet(MinerDB_t *queue);

int DatabaseSize(MinerDB_t * queue);

void lockQueue(MinerDB_t *queue);
void unlockQueue(MinerDB_t *queue);

void semPut(MinerDB_t *queue);
void semGet(MinerDB_t *queue);

#endif