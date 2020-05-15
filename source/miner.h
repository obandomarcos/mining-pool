#ifndef MINER_H_
#define MINER_H_
#include "packet.h"

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

typedef struct Pool
{
    int minersock;
    struct hostent *pool;
    struct sockaddr_in pool_addr;
    // esto en un futuro será una cola de mineros
          // minero actual? podría ser
    // Futuro, mineros que se conecten entre si
    // struct sockaddr_in miner_addr;    

} Miner_t;

Miner_t * minerCreate();
void minerInit(Miner_t * miner, char * pool_name);
void minerDestroy(Miner_t *miner);
void minerSendPacket(Miner_t * miner, PacketType_t pType);
void minerProcessPacket(Miner_t * miner);

#endif // MINER_H_