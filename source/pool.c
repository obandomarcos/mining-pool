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

Pool_t * poolInit()
{

    Pool_t * pool = (Pool_t *)malloc(sizeof(Pool_t));
    CHECK(pool != NULL);

    return pool;
}

void poolListen(Pool_t * pool)
{   
    
    pool -> poolsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(pool -> poolsock!=-1);

    memset(&pool -> pool_addr, 0, sizeof(pool -> pool_addr));
    pool -> pool_addr.sin_family = AF_INET;
    pool -> pool_addr.sin_port = htons(PORT);
    pool -> pool_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    CHECK(bind(pool -> poolsock, (struct sockaddr *)&pool -> pool_addr, sizeof(pool -> pool_addr))!=-1);

    printf("Escuchando desde el pool por nuevo minero\n");

}

void poolDestroy(Pool_t * pool){

    close(pool -> poolsock);

    free(pool);

}

// esta sería como un método privado
void poolSendPacket(Pool_t *pool, PacketType_t pType)
{
    Packet_t packet;
    
    packet.type = pType;
    packet.sz8 = sizeof(packet.sz8)+sizeof(packet.type);

    switch (pType)
    {
        case welcomeMiner:
        
            packet.sz8 += sizeof(packet.args.args_welcomeMiner);
            strcpy(packet.args.args_welcomeMiner.mensaje, "¡Bienvenido a MinaSim!\n");
            
            CHECK(sendto(pool -> poolsock, &packet, packet.sz8, 0, (struct sockaddr *)&pool -> miner_addr, (socklen_t)sizeof(pool -> miner_addr))!=-1);
            break;

        case farewellMiner:
        
            packet.sz8 += sizeof(packet.args.args_connectPool);
            strcpy(packet.args.args_farewellMiner.mensaje, "Te saludamos desde MinaSim!\n");

            CHECK(sendto(pool -> poolsock, &packet, packet.sz8, 0, (struct sockaddr *)&pool -> miner_addr, (socklen_t)sizeof(pool -> miner_addr))!=-1);
            break;
        
        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }
}

void poolProcessPacket(Pool_t *pool){

    Packet_t packet;
    socklen_t slen = sizeof(pool -> miner_addr);

    CHECK(recvfrom(pool -> poolsock, &packet, sizeof(Packet_t), 0, (struct sockaddr *)&pool -> miner_addr, &slen) != -1);
    
    switch (packet.type)
    {
        case connectPool:
            
            printf("%s", packet.args.args_welcomeMiner.mensaje);
            
            // cargar a la lista de mineros

            poolSendPacket(pool, welcomeMiner);
            break;

        case disconnectPool:

            poolSendPacket(pool, farewellMiner);

            // getear de la lista de mineros
            printf("%s", packet.args.args_disconnectPool.mensaje);
            break;
        
        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }
    
    printf("Paquete recibido desde %s:%d\n\n", 
    inet_ntoa(pool -> miner_addr.sin_addr), ntohs(pool -> miner_addr.sin_port));
}