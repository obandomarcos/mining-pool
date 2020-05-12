#ifndef POOL_H_
#define POOL_H_

#include <mqueue.h>
#include <sys/socket.h>

#include "miner.h"
#include "database"

typedef unsigned int MinerId;
typedef unsigned int uint;

// MinerReg_t allows to have a registry of miners connected to the pool
typedef struct {

    MinerId id;
    int wallet;
    struct sockaddr miner_address;

} MinerReg_t;

typedef struct{

    MinerDb_t *minerDb;

}   Pool_t;

// Creación, inicialización y destrucción
Pool_t * poolInit();
void poolDestroy(Pool_t *pool);

// Proceso mensajes enviados por el minero
void pProcessPacket(Pool_t* pool, Packet_t *packet);
void pFillPacket(Pool_t* pool, Packet_t * packet);

#endif //