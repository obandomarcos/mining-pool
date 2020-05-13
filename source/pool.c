#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"
#include "packet.h"
#include "pool.h"

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

int poolInit(struct sockaddr_in *si_pool)
{   
    int sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(sockfd!=-1);

    memset(si_pool, 0, sizeof(*si_pool));
    si_pool->sin_family = AF_INET;
    si_pool->sin_port = htons(PORT);
    si_pool->sin_addr.s_addr = htonl(INADDR_ANY);
    
    CHECK(bind(sockfd, (struct sockaddr *)si_pool, sizeof(*si_pool))!=-1);

    printf("Escuchando desde el pool por nuevo minero\n");
    return sockfd;
}

void poolDestroy(int poolsock){

    close(poolsock);
}

void poolSendPacket(int poolsock, struct sockaddr_in * miner_addr, PacketType_t pType){

    Packet_t packet;
    socklen_t slen=sizeof(*miner_addr);
    
    packet.type = pType;
    packet.sz8 = sizeof(packet.sz8)+sizeof(packet.type);

    switch (pType)
    {
        case welcomeMiner:
        
            packet.sz8 += sizeof(packet.args.args_welcomeMiner);
            strcpy(packet.args.args_welcomeMiner.mensaje, "¡Bienvenido a MinaSim!\n");
            
            break;

        case farewellMiner:
            
            packet.type = farewellMiner;
        
            packet.sz8 += sizeof(packet.args.args_connectPool);
            strcpy(packet.args.args_farewellMiner.mensaje, "Te saludamos desde MinaSim!\n");
            break;
        
        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }

    CHECK(sendto(poolsock, &packet, packet.sz8, 0, (struct sockaddr *)miner_addr, slen)!=-1);
}

void poolProcessPacket(int poolsock, struct sockaddr_in *miner_addr){

    Packet_t packet;
    socklen_t slen=sizeof(*miner_addr);

    CHECK(recvfrom(poolsock, &packet, sizeof(Packet_t), 0, (struct sockaddr *)miner_addr, &slen)!=-1);
    
    switch (packet.type)
    {
        case connectPool:
            
            printf("%s", packet.args.args_welcomeMiner.mensaje);
            
            poolSendPacket(poolsock, miner_addr, welcomeMiner);
            break;

        case disconnectPool:

            poolSendPacket(poolsock, miner_addr, farewellMiner);

            printf("%s", packet.args.args_disconnectPool.mensaje);
            break;
        
        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }
    
    printf("Paquete recibido desde %s:%d\n\n", 
    inet_ntoa(miner_addr -> sin_addr), ntohs(miner_addr -> sin_port));
}