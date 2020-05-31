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


int main(int argc, char *argv[])
{   
    Miner_t *miner;

    CHECK(argc > 2);
    
    miner = minerCreate(argv[2]);
    minerInit(miner, argv[1]);
    
    printf("Bienvenido a Bandis - Minero número %d\n\n", atoi(argv[2]));

    printf("Command Reference\n\n0 - Connect to Pool\n4 - Show platita\n5 - Close miner\n6 - Help\n");

    while(miner->active)
    {   
        minerProcessPacket(miner);
    }
    
    minerDestroy(miner);
    
    return 0;
}