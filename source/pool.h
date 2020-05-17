#ifndef POOL_H_
#define POOL_H_

#include "packet.h"

#define BUFLEN 512
#define NPACK 10
#define PORT 9930
#define MCAST_GROUP "225.0.0.37"
#define MCAST_PORT 12345

typedef struct Pool
{
    int poolsock;
    struct sockaddr_in pool_addr;

    int miners;
    // esto en un futuro será una cola de mineros
    struct sockaddr_in miner_addr;      // minero actual? podría ser
    struct sockaddr_in mcast_addr;

    //esto debe ser un bloque
    int currentBlock;
    int32_t totRepartido;
    int32_t nonce;

} Pool_t;

Pool_t * poolInit();
void poolListen(Pool_t *pool);
void poolDestroy(Pool_t * pool);
void poolSendPacket(Pool_t *pool, PacketType_t pType);
void poolProcessPacket(Pool_t *pool);
void poolVerifyBlock(Pool_t * pool, int32_t goldNonce);
uint32_t calculateReward(Pool_t *pool);


#endif //PACKET_H_