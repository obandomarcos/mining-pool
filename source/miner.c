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

Miner_t * minerCreate()
{

    Miner_t * miner = (Miner_t *)malloc(sizeof(Miner_t));
    CHECK(miner != NULL);

    return miner;
}

void minerInit(Miner_t * miner, char * pool_name)
{   
    miner -> minersock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(miner -> minersock);

    miner -> pool = gethostbyname(pool_name);
    CHECK(miner -> pool!=NULL);

    memset(&miner -> pool_addr, 0, sizeof(miner -> pool_addr));
    miner -> pool_addr.sin_family = AF_INET;

    // con host ent podría conectarme a múltiples aaddresseeeees!!!!
    memcpy(&miner -> pool -> h_addr, &miner -> pool_addr.sin_addr.s_addr, miner-> pool -> h_length);
    miner -> pool_addr.sin_port = htons(PORT);

}

void minerDestroy(Miner_t *miner){

    close(miner -> minersock);

    free(miner);
}

void minerSendPacket(Miner_t * miner, PacketType_t pType){

    Packet_t packet;
    socklen_t slen=sizeof(miner -> pool_addr);

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

    CHECK(sendto(miner -> minersock, &packet, packet.sz8, 0, (struct sockaddr *)&miner -> pool_addr, slen)!=-1);
}

void minerProcessPacket(Miner_t * miner){

    Packet_t packet;
    socklen_t slen = sizeof(miner -> pool_addr);

    CHECK(recvfrom(miner -> minersock, &packet, sizeof(Packet_t), 0, (struct sockaddr *)&miner -> pool_addr, &slen)!=-1);
    
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