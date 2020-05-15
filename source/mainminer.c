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
    char c;
    Miner_t *miner = minerCreate();

    CHECK(argc >= 2);

    minerInit(miner, argv[1]);

    //conexión

    minerSendPacket(miner, connectPool);

    minerProcessPacket(miner);

    c = getchar();
    printf("Entrada %c\n", c);
    //desconexión
    
    minerSendPacket(miner, disconnectPool);

    minerProcessPacket(miner);

    minerDestroy(miner);
    return 0;
}

