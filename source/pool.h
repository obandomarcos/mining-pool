#ifndef POOL_H_
#define POOL_H_

#include "packet.h"

int poolInit(struct sockaddr_in *si_pool);
void poolDestroy(int poolsock);
void poolSendPacket(int poolsock, struct sockaddr_in * miner_addr, PacketType_t pType);
void poolProcessPacket(int poolsock, struct sockaddr_in *miner_addr);

#endif //PACKET_H_