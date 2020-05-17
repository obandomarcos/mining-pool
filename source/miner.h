#ifndef MINER_H_
#define MINER_H_
#include "packet.h"

#define BUFLEN 512
#define NPACK 10
#define PORT 9930
#define MCAST_GROUP "225.0.0.37"
#define MCAST_PORT 12345

typedef struct Miner
{
    int minersock;
    int mcastsock;
    struct hostent *pool;
    struct sockaddr_in pool_addr;
    
    // MULTICAST
    struct sockaddr_in mcast_addr;
    struct ip_mreqn group;

    // ELEMENTOS DEL MINERO

    int nonce;
    int block;
    
    uint32_t wallet;
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
Packet_t packetRetrieval(Miner_t *miner);
int max(int x, int y);

#endif // MINER_H_