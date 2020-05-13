#ifndef MINER_H_
#define MINER_H_
#include "packet.h"

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

int minerInit(char *pool_name, struct hostent *pool, struct sockaddr_in *pool_addr);
void minerDestroy(int minersock);
void minerSendPacket(int minersock, struct sockaddr_in *pool_addr, PacketType_t pType);
void minerProcessPacket(int minersock, struct sockaddr_in *pool_addr);

#endif // MINER_H_