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
#include "common.h"
#include "packet.h"
#include "miner.h"

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

int minerInit(char *pool_name, struct hostent *pool, struct sockaddr_in *pool_addr){
    
    int sockfd;

    CHECK(sockfd=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP));

    pool = gethostbyname(pool_name);
    CHECK(pool!=NULL);

    memset(pool_addr, 0, sizeof(*pool_addr));
    pool_addr -> sin_family = AF_INET;
    memcpy(pool->h_addr, &pool_addr->sin_addr.s_addr, pool->h_length);
    pool_addr->sin_port = htons(PORT);

    return sockfd;
}

void minerDestroy(int minersock){

    close(minersock);
}

void minerSendPacket(int minersock, struct sockaddr_in *pool_addr, PacketType_t pType){

    Packet_t packet;
    socklen_t slen=sizeof(*pool_addr);

    packet.type = pType;
    packet.sz8 = sizeof(packet.sz8)+sizeof(packet.type);

    switch (pType)
    {
        case connectPool:
        
            packet.sz8 += sizeof(packet.args.args_connectPool);
            strcpy(packet.args.args_connectPool.mensaje, "Buen día, quiero conectarme al pool\n");
            
            break;

        case disconnectPool:

            packet.sz8 += sizeof(packet.args.args_disconnectPool);
            strcpy(packet.args.args_farewellMiner.mensaje, "Me hinche las bolas, renuncio\n");
            
            break;
        
        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }

    CHECK(sendto(minersock, &packet, packet.sz8, 0, (struct sockaddr *)pool_addr, slen)!=-1);
}

void minerProcessPacket(int minersock, struct sockaddr_in *pool_addr){

    Packet_t packet;
    socklen_t slen=sizeof(*pool_addr);

    CHECK(recvfrom(minersock, &packet, sizeof(Packet_t), 0, (struct sockaddr *)pool_addr, &slen)!=-1);
    
    switch (packet.type)
    {
        case welcomeMiner:

            printf("%s", packet.args.args_welcomeMiner.mensaje);
            break;

        case farewellMiner:
            
            printf("%s", packet.args.args_farewellMiner.mensaje);
            break;
        
        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }
}