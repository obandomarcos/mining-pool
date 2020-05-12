#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <netinet/in.h> 

#include "common.h"
#include "database.h"

#define STORAGE_ID "/DATABASE_TESTING"
#define STORAGE_SIZE 1024
#define MAX_QUEUE_ELEMENTS 5

// PONER DOCUMENTACIÓN AQUÍ
MinerDB_t* DatabaseCreate(){

    // inicializo un objeto  POSIX de memoria compartida
    int ret, fd;

    size_t size = sizeof(MinerDB_t)+MAX_QUEUE_ELEMENTS*sizeof(MinerReg_t);
    MinerDB_t *queue;
    
    fd = shm_open(STORAGE_ID, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    CHECK(fd != -1);

    ret = ftruncate(fd, size);
	CHECK(ret == 0);

    queue = mmap(NULL, size, proteccion, visibilidad, fd, 0);
    assert(queue!= MAP_FAILED);

    queue -> qSize = MAX_QUEUE_ELEMENTS;
    
    queue -> getter = 0;
    queue -> putter = 0;
    
    ret = sem_init(&queue -> sem_get, 1, 0);
    CHECK(ret != -1);
    
    ret = sem_init(&queue -> sem_put, 1, MAX_QUEUE_ELEMENTS);
    CHECK(ret != -1);

    ret = sem_init(&queue -> sem_lock, 1, 1);
    CHECK(ret != -1);

    return queue;
}

void DatabaseDestroy(MinerDB_t *queue){

    int fd;
    
    fd = sem_destroy(&queue-> sem_get);
    CHECK(fd != -1);

    fd = sem_destroy(&queue-> sem_put);
    CHECK(fd != -1);
    
    fd = sem_destroy(&queue-> sem_lock);
    CHECK(fd != -1);

    fd = munmap(queue, sizeof(MinerDB_t)+MAX_QUEUE_ELEMENTS*sizeof(int));
    CHECK(fd != -1);
    
    fd = shm_unlink(STORAGE_ID);
	CHECK(fd != -1);

    return;
}

void DatabasePut(MinerDB_t *queue, MinerReg_t elem){

    // bloqueo el acceso a la cola
    lockQueue(queue);
    // si no puedo poner un elemento más, se bloquea
    semPut(queue);

    queue -> miners[queue -> putter].id = elem.id;
    queue -> miners[queue -> putter].wallet = elem.wallet;
    
    // revisar esto
    queue -> miners[queue -> putter].miner_addr = elem.miner_addr;

    // sumo uno al putter y lo modulo con numero de elementos
    queue -> putter += 1;
    queue -> putter %= queue-> qSize; 

    unlockQueue(queue);  
}


MinerReg_t DatabaseGet(MinerDB_t *queue){

    MinerReg_t retval;
    // bloqueo el acceso a la cola
    lockQueue(queue);
    // si no puedo sacar un elemento más, se bloquea
    semGet(queue);

    retval = queue -> miners[queue -> getter];
    // sumo uno al getter y lo modulo con numero de elementos
    queue -> getter++;
    queue -> getter %= queue-> qSize; 

    unlockQueue(queue);  

    return retval;
}

int DatabaseSize(MinerDB_t * queue){

    int dif = (queue -> putter - queue -> getter);

    // separo en los dos casos, a menos que sea cero y ahí la cola esta como el orto, valga la redundancia

    if(dif > 0 ){

        return dif;
    }

    else if(dif < 0){

        return MAX_QUEUE_ELEMENTS + dif;
    }
    else {
        perror("Cola entro mal");
        exit(1);
    }
}

// MÁS DOCUMENTACIÓN

void lockQueue(MinerDB_t *queue){

    while(sem_wait(&queue->sem_lock) ==-1 && errno == EINTR);
}

void unlockQueue(MinerDB_t *queue){

    sem_post(&queue->sem_lock);
}

void semPut(MinerDB_t *queue){

    sem_post(&queue -> sem_get);
    while(sem_wait(&(queue->sem_put)) ==-1 && errno == EINTR);
}

void semGet(MinerDB_t *queue){

    sem_post(&queue->sem_put);
    while(sem_wait(&queue->sem_get) ==-1 && errno == EINTR);
}