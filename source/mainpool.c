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

int main(void)
{
    struct sockaddr_in si_pool, si_miner;
    int s;

    // inicializo al pool, protocolo UDP, ver broadcast
    s = poolInit(&si_pool); 

    while(1){
    // Recepción de paquetes
        poolProcessPacket(s, &si_miner);
    }

    poolDestroy(s);
    return 0;
}