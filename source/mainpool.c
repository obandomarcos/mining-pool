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
    Pool_t *pool = poolInit();

    poolListen(pool); 

    printf("Bienvenido a Bandis - Pool server\n\nCommand Reference\n\n0 - Shutdown Pool\n1 - Show platucha\n2 - Check current number of miners\n");
    
    printf("Recibiendo conexiones\n");

    while(pool->active){
    // Recepción de paquetes
        poolProcessPacket(pool);
    }

    poolDestroy(pool);
    return 0;
}

