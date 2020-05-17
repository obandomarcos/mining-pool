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

void rutina1(Miner_t *miner);
void rutina2(Miner_t *miner);
void rutina3(Miner_t *miner);

int main(int argc, char *argv[])
{   
    Miner_t *miner = minerCreate();

    CHECK(argc >= 2);

    minerInit(miner, argv[1]);

    //conexión
    rutina3(miner);

    minerDestroy(miner);
    return 0;
}


void rutina3(Miner_t *miner){

    minerSendPacket(miner, connectPool);

    minerProcessPacket(miner);

    minerSendPacket(miner, reqBlock);

    minerProcessPacket(miner);

    minerSendPacket(miner, reqNonce);

    minerProcessPacket(miner);
    
    // acá laburo e imprimo

    minerSendPacket(miner, disconnectPool);

    minerProcessPacket(miner);
}

void rutina2(Miner_t *miner){

    minerSendPacket(miner, connectPool);

    minerProcessPacket(miner);

    minerProcessPacket(miner);
}
void rutina1(Miner_t *miner)
{   
    char c;

    minerSendPacket(miner, connectPool);

    minerProcessPacket(miner);

    c = getchar();
    printf("%c\n", c);

    // pido block
    minerSendPacket(miner, reqBlock);

    minerProcessPacket(miner);

    c = getchar();
    printf("Bloque %d\n", miner -> block);

    // pido nonce
    minerSendPacket(miner, reqBlock);

    minerProcessPacket(miner);

    c = getchar();
    printf("Nonce %d\n", miner -> nonce);

    miner -> nonce = 12;
    // mando nonce
    minerSendPacket(miner, submitNonce);

    minerProcessPacket(miner);

    c = getchar();
    printf("Entrada %c\n", c);

    //desconexión
    
    minerSendPacket(miner, disconnectPool);

    minerProcessPacket(miner);
}