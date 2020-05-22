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

void rutina4(Miner_t *miner);
void connMiner(Miner_t *miner);
void discMiner(Miner_t *miner);
void reqblockMiner(Miner_t * miner);
void reqnonceMiner(Miner_t * miner);

int main(int argc, char *argv[])
{   
    Miner_t *miner = minerCreate();

    CHECK(argc >= 2);

    minerInit(miner, argv[1]);

    minerSendPacket(miner, connectPool);
    minerProcessPacket(miner);
    printf("%d\n", miner->active);
    while(miner->active)
    {   
        printf("%d\n", miner->active);
        minerProcessPacket(miner);
    }
    
    minerDestroy(miner);
    
    return 0;
}

void connMiner(Miner_t *miner)
{
    minerSendPacket(miner, connectPool);
}

void reqblockMiner(Miner_t * miner)
{   
    minerSendPacket(miner, reqBlock);
}

void reqnonceMiner(Miner_t * miner)
{   
    minerSendPacket(miner, reqNonce);
}

void discMiner(Miner_t *miner)
{
    minerSendPacket(miner, disconnectPool);
    minerProcessPacket(miner);
}

void stateMiner(Miner_t *miner)
{   
    if (miner -> goldNonce != -1)
    {
        printf("Lo encontramos\n");
        minerSendPacket(miner, submitNonce);
        // successBlock
        minerProcessPacket(miner);
        // flood stop, esto lo quisiera escuchar en otro proceso
        minerProcessPacket(miner);
    }
    else 
    {
        printf("Ja la estoy cagando\n");
        reqnonceMiner(miner);
    }
}

void rutina4(Miner_t *miner)
{

    // me conecto
    connMiner(miner);
    reqblockMiner(miner);

    while(miner -> active){
        
        reqnonceMiner(miner);
        stateMiner(miner);
        printf("%d\n", miner->active);
    }

    // recibo recompensa y me desconecto
    minerProcessPacket(miner);

    printf("%d\n", miner->wallet);
    discMiner(miner);
}