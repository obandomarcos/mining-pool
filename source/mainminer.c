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
    int s;
    struct sockaddr_in pool_addr;
    struct hostent *pool = NULL;
    char c;

    CHECK(argc >= 2);

    s = minerInit(argv[1], pool, &pool_addr);

    //conexión

    minerSendPacket(s, &pool_addr, connectPool);

    minerProcessPacket(s, &pool_addr);

    c = getchar();
    
    //desconexión

    minerSendPacket(s, &pool_addr, disconnectPool);

    // Recepción de paquetes
    minerProcessPacket(s, &pool_addr);

    minerDestroy(s);
    return 0;
}

