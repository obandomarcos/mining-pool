#ifndef POOL_H_
#define POOL_H_

#include "packet.h"

typedef struct Pool
{
    int poolsock;
    struct sockaddr_in pool_addr;
    // esto en un futuro será una cola de mineros
    struct sockaddr_in miner_addr;      // minero actual? podría ser
    
} Pool_t;

Pool_t * poolInit();
void poolListen(Pool_t *pool);
void poolDestroy(Pool_t * pool);
void poolSendPacket(Pool_t *pool, PacketType_t pType);
void poolProcessPacket(Pool_t *pool);

#endif //PACKET_H_