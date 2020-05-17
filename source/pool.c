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

Pool_t * poolInit()
{

    Pool_t * pool = (Pool_t *)malloc(sizeof(Pool_t));
    CHECK(pool != NULL);
    
    pool -> currentBlock = 42;
    pool -> totRepartido = 1;
    pool -> nonce = 0;
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

    // como el otro lo tengo conectado a todas las interfaces, no me quiero mandar cagadas
    memset(&pool -> mcast_addr,0,sizeof(pool -> mcast_addr));
    pool -> mcast_addr.sin_family = AF_INET;
    pool -> mcast_addr.sin_addr.s_addr = inet_addr(MCAST_GROUP);
    pool -> mcast_addr.sin_port = htons(MCAST_PORT);    

    printf("Escuchando desde el pool por nuevo minero\n");

}

void poolDestroy(Pool_t * pool)
{

    close(pool -> poolsock);

    free(pool);
}

// esta sería como un método privado
void poolSendPacket(Pool_t *pool, PacketType_t pType)
{
    Packet_t packet;
    
    packet.type = pType;
    packet.sz8 = sizeof(packet.sz8)+sizeof(packet.type);

    // chetear las address y mandarlo luego de empaquetar mierda
    switch (pType)
    {
        case welcomeMiner:
        
            packet.sz8 += sizeof(packet.args.args_welcomeMiner);
            strcpy(packet.args.args_welcomeMiner.mensaje, "¡Bienvenido a MinaSim!\n");
            
            CHECK(sendto(pool -> poolsock, &packet, packet.sz8, 0, (struct sockaddr *)&pool -> miner_addr, (socklen_t)sizeof(pool -> miner_addr))!=-1);
            break;

        case farewellMiner:
        
            packet.sz8 += sizeof(packet.args.args_farewellMiner);
            strcpy(packet.args.args_farewellMiner.mensaje, "Te saludamos desde MinaSim!\n");

            CHECK(sendto(pool -> poolsock, &packet, packet.sz8, 0, (struct sockaddr *)&pool -> miner_addr, (socklen_t)sizeof(pool -> miner_addr))!=-1);
            break;

        case sendBlock:

            packet.sz8 += sizeof(packet.args.args_sendBlock);

            //función get nonce
            packet.args.args_sendBlock.block = pool-> currentBlock; 

            CHECK(sendto(pool -> poolsock, &packet, packet.sz8, 0, (struct sockaddr *)&pool -> miner_addr, (socklen_t)sizeof(pool -> miner_addr))!=-1);

            break;
        case sendNonce:

            packet.sz8 += sizeof(packet.args.args_sendNonce);

            //función get nonce
            packet.args.args_sendNonce.nonce = 10;
            packet.args.args_sendNonce.section = 20; 

            CHECK(sendto(pool -> poolsock, &packet, packet.sz8, 0, (struct sockaddr *)&pool -> miner_addr, (socklen_t)sizeof(pool -> miner_addr))!=-1);
            break;
        
        case discardBlock:
            // logear el descarte
            poolSendPacket(pool, sendNonce);
            break;

        case failureBlock:
            // logear fallos , it's weird
            poolSendPacket(pool, sendNonce);
            break;

        case successBlock:

            poolSendPacket(pool, sendReward);
            break;

        // multicast acá

        case floodStop : 
            
            packet.sz8 += sizeof(packet.args.args_floodStop);
            
            //función get nonce
            strcpy(packet.args.args_floodStop.mensaje, "Basta de laburar\n");
            
            CHECK(sendto(pool -> poolsock, &packet, packet.sz8, 0, (struct sockaddr *)&pool -> mcast_addr, (socklen_t)sizeof(pool -> mcast_addr))!=-1);

            break;

        // case sendReward:

        //     packet.sz8 += sizeof(packet.args.args_sendReward);
            
        //     // el total repartido no siempre podría ser el ganado por el pool
        //     packet.args.args_sendReward.norm = pool -> totRepartido;
        //     packet.args.args_sendReward.reward = calculateReward(pool);

        //     break;

        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }
}

void poolProcessPacket(Pool_t *pool)
{

    Packet_t packet;
    socklen_t slen = sizeof(pool -> miner_addr);

    CHECK(recvfrom(pool -> poolsock, &packet, sizeof(Packet_t), 0, (struct sockaddr *)&pool -> miner_addr, &slen) != -1);
    
    switch (packet.type)
    {
        case connectPool:
            
            printf("%s", packet.args.args_welcomeMiner.mensaje);
            
            // cargar a la lista de mineross

            poolSendPacket(pool, welcomeMiner);
            break;

        case disconnectPool:

            poolSendPacket(pool, farewellMiner);

            // getear de la lista de mineros
            printf("%s", packet.args.args_disconnectPool.mensaje);
            break;
        
        case reqNonce:

            poolSendPacket(pool, sendNonce); 
            break;

        case reqBlock:

            poolSendPacket(pool, sendBlock); 
            break;

        case submitNonce:

            //acá voy a decidir en base al bloque si es bueno o malo y le mando la confirmación, también guardo el address
            poolVerifyBlock(pool, packet.args.args_submitNonce.goldNonce);
            // cambiar esto despues
            poolSendPacket(pool, floodStop);
            break;

        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }
    
    printf("Paquete recibido desde %s:%d\n\n", 
    inet_ntoa(pool -> miner_addr.sin_addr), ntohs(pool -> miner_addr.sin_port));
}

void poolVerifyBlock(Pool_t * pool, int32_t goldNonce)
{   
    pool -> nonce = goldNonce;
    printf("Hacer función\n");
}

uint32_t calculateReward(Pool_t *pool)
{
    uint32_t reward = 1;
    
    printf("Hacer función\n");
    
    return reward;
}