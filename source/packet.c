#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h> 

/// concentrarme mas tarde en esto


Packet_t minerPacketCreate(Miner_t *miner)
{       
    Packet_t * packet = (Packet_t *)malloc(sizeof(Packet_t));  
    socklen_t slen = sizeof(miner -> pool_addr);


    CHECK(recvfrom(miner -> minersock, packet, sizeof(Packet_t), 0, (struct sockaddr *)&miner -> pool_addr, &slen)!=-1);

    return packet;
}

Packet_t poolPacketCreate(Miner_t *miner)
{       
    Packet_t * packet = (Packet_t *)malloc(sizeof(Packet_t));  
    socklen_t slen = sizeof(miner -> pool_addr);


    CHECK(recvfrom(miner -> minersock, packet, sizeof(Packet_t), 0, (struct sockaddr *)&miner -> pool_addr, &slen)!=-1);

    return packet;
}

void packetDestroy(Packet_t *packet){

    free(packet);
}
