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
    uint yes = 1;
    struct timeval read_timeout;

    miner -> minersock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(miner -> minersock);

    miner -> pool = gethostbyname(pool_name);
    CHECK(miner -> pool!=NULL);

    memset(&miner -> pool_addr, 0, sizeof(miner -> pool_addr));
    miner -> pool_addr.sin_family = AF_INET;

    memcpy(&miner -> pool -> h_addr, &miner -> pool_addr.sin_addr.s_addr, miner-> pool -> h_length);
    miner -> pool_addr.sin_port = htons(PORT);

    // modifico el socket para recibir multicast (estaría bueno recibir la dirección de multicast de parte del minero y que se configure)



    miner -> mcastsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(miner -> mcastsock);

    // timeout
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 10;
    
    CHECK(setsockopt(miner -> mcastsock, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout)) >= 0);

    CHECK(setsockopt(miner -> mcastsock, SOL_SOCKET, SO_REUSEADDR,&yes,sizeof(yes)) >= 0);

    memset(&miner -> mcast_addr, 0, sizeof(miner -> mcast_addr));

    miner -> mcast_addr.sin_family = AF_INET;
    miner -> mcast_addr.sin_port = htons(MCAST_PORT);
    miner -> mcast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    CHECK(bind(miner -> mcastsock, (struct sockaddr*)&miner -> mcast_addr, sizeof(miner -> mcast_addr))== 0);

    // me uno al grupo multicast
    miner -> group.imr_multiaddr.s_addr = inet_addr(MCAST_GROUP);
    miner -> group.imr_address.s_addr = htonl(INADDR_ANY);
    miner -> group.imr_ifindex = 0;

    CHECK(setsockopt(miner -> mcastsock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&miner->group,sizeof(miner->group)) >= 0);

}

void minerDestroy(Miner_t *miner)
{

    close(miner -> minersock);

    free(miner);
}

void minerSendPacket(Miner_t * miner, PacketType_t pType)
{

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

        case reqNonce:

            packet.sz8 += sizeof(packet.args.args_reqNonce);
            
            break;

        case reqBlock:

            packet.sz8 += sizeof(packet.args.args_reqBlock);
            // algo de info sobre el bloque que pido
            break;
        
        case submitNonce:

            packet.sz8 += sizeof(packet.args.args_submitNonce);
            strcpy(packet.args.args_submitNonce.mensaje, "Encontré un blocardo\n");
            packet.args.args_submitNonce.goldNonce = (miner -> nonce+10);

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

void minerProcessPacket(Miner_t * miner)
{

    Packet_t packet;
    
    packet = selectMachine(miner);

    switch (packet.type)
    {
        case welcomeMiner:

            printf("%s", packet.args.args_welcomeMiner.mensaje);
            break;

        case sendNonce:

            // sigo laburando no matter what
            miner -> nonce = packet.args.args_sendNonce.nonce;
        
        case sendBlock:

            miner -> block = packet.args.args_sendBlock.block;

        case sendReward :

            miner->wallet += packet.args.args_sendReward.reward;

        case farewellMiner:
            
            printf("%s", packet.args.args_farewellMiner.mensaje);
            break;
        
        case floodStop:

            printf("%s\n", packet.args.args_floodStop.mensaje );
            break;
            
        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }
}

Packet_t packetRetrieval(Miner_t *miner){

    Packet_t packet;
    int ready, maxfdp;
    fd_set rset;
    socklen_t slenPool = sizeof(miner -> pool_addr);
    socklen_t slenMcast = sizeof(miner -> mcast_addr);


    // si anda, timeout
    FD_ZERO(&rset);

    maxfdp = max(miner -> minersock, miner -> mcastsock) + 1; 

    while(1)
    {   
        FD_SET(miner -> minersock, &rset); 
        FD_SET(miner -> mcastsock, &rset);

        ready = select(maxfdp, &rset, NULL, NULL, NULL); 
        CHECK(ready >= 0);
        //unicast
        if (FD_ISSET(miner -> minersock, &rset)) 
        { 
            // non comprehensive sanity check acá gilardo
            CHECK(recvfrom(miner -> minersock, &packet, sizeof(packet), 0, (struct sockaddr*)&miner -> pool_addr , &slenPool) !=  -1); 
            break;
        } 

        if (FD_ISSET(miner -> mcastsock, &rset)) 
        { 
            // non comprehensive sanity check acá gilardo
            CHECK(recvfrom(miner -> mcastsock, &packet, sizeof(packet), 0, (struct sockaddr*)&miner -> mcast_addr , &slenMcast) !=  -1); 
            break;
        } 
    }

    return packet;
}

int max(int x, int y) 
{ 
    if (x > y) 
        return x; 
    else
        return y; 
} 