#ifndef MINER_H_
#define MINER_H_

#include <math.h>

#include "packet.h"
#include "workers.h"

#define BUFLEN 512
#define NPACK 10
#define PORT 9930
#define MCAST_GROUP "225.0.0.37"
#define MCAST_PORT 12345



typedef struct Miner
{
    int minersock;
    int mcastsock;

    // SERVER ADDRESS
    struct hostent *pool;
    struct sockaddr_in pool_addr;
    
    // MULTICAST
    struct sockaddr_in mcast_addr;
    struct ip_mreqn group;

    // MINA

    int nonce;
    int section;
    int block;
    
    // TRABAJADORES Y COLA DE TRABAJO
    Worker_t *workers;
    mqd_t reqQueue; 

    uint32_t wallet;
    // esto en un futuro será una cola de mineros
          // minero actual? podría ser
    // Futuro, mineros que se conecten entre si
    // struct sockaddr_in miner_addr;    

} Miner_t;

// Creación y destrucción
Miner_t * minerCreate();
void minerDestroy(Miner_t *miner);

// Protocolo de comunicación
void minerInit(Miner_t * miner, char * pool_name);
void minerSendPacket(Miner_t * miner, PacketType_t pType);
void minerProcessPacket(Miner_t * miner);
Packet_t packetRetrieval(Miner_t *miner);

// Funcionamiento : Inicio y finalización de busqueda de Hash
void minerLoadQueue(Miner_t *miner, int workerQty);

// Funciones útiles
int max(int x, int y);
void hashBlock(void *ctx);

#endif // MINER_H_