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

void rutina1Pool(Pool_t *pool);
void rutina2Pool(Pool_t *pool);

int main(void)
{
    Pool_t *pool = poolInit();

    poolListen(pool); 

    rutina1Pool(pool);

    poolDestroy(pool);
    return 0;
}

void rutina1Pool(Pool_t *pool){
    
    while(1){
    // Recepción de paquetes
        poolProcessPacket(pool);
    }
    
}
